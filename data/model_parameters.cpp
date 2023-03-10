//
// Created by jay on 2023/1/2.
//

#include "../data/model_parameters.h"

#include <string.h>
#include <pmmintrin.h>  // for SSE
#include <stdio.h>
#include <malloc.h>
#include "../base/file_util.h"
#include "../base/format_print.h"
#include "../base/math.h"
#include "../base/logging.h"
#include "../base/stringprintf.h"


namespace xLearn {
//------------------------------------------------------------------------------
// The Model class
//------------------------------------------------------------------------------

// Basic contributor.
    void Model::Initialize(const std::string& score_func,
                           const std::string& loss_func,
                           index_t num_feature,
                           index_t num_field,
                           index_t num_K,
                           index_t aux_size,
                           real_t scale) {
        CHECK(!score_func.empty());
        CHECK(!loss_func.empty());
        CHECK_GT(num_feature, 0);
        CHECK_GE(num_field, 0);
        CHECK_GE(num_K, 0);
        // The aux_size is used for different
        // optimization method
        CHECK_GE(aux_size, 0);
        CHECK_GT(scale, 0);
        score_func_ = score_func;
        loss_func_ = loss_func;
        num_feat_ = num_feature;
        num_field_ = num_field;
        num_K_ = num_K;
        aux_size_ = aux_size;
        scale_ = scale;
        // Calculate the number of model parameters
        param_num_w_ = num_feature * aux_size_;
        if (score_func == "linear") {
            param_num_v_ = 0;
        } else if (score_func == "fm") {
            // fm: feature * K
            param_num_v_ = num_feature * get_aligned_k() * aux_size_;
        } else if (score_func == "ffm") {
            // ffm: feature * K * field
            param_num_v_ = num_feature * get_aligned_k() * num_field * aux_size_;
        } else {
            LOG(FATAL) << "Unknow score function: " << score_func;
        }
        this->initial(true);
    }

// To get the best performance for SSE, we need to
// allocate memory for the model parameters in aligned way.
// For SSE, the align number should be 16 byte.
    void Model::initial(bool set_val) {
        try {
            // Conventional malloc for linear term and bias
            param_w_ = (real_t*)malloc(param_num_w_ * sizeof(real_t));
            param_b_ = (real_t*)malloc(aux_size_ * sizeof(real_t));
            if (score_func_.compare("fm") == 0 ||
                score_func_.compare("ffm") == 0) {
                // Aligned malloc for latent factor
#ifdef _WIN32
                param_v_ = (xLearn::real_t *)_aligned_malloc(param_num_v_ * sizeof(real_t), kAlignByte);
#else
                int ret = posix_memalign(
                (void**)&param_v_,
                kAlignByte,
                param_num_v_ * sizeof(real_t));
      CHECK_EQ(ret, 0);
#endif
            } else {
                param_v_ = nullptr;
            }
        } catch (std::bad_alloc&) {
            LOG(FATAL) << "Cannot allocate enough memory for current  \
                   model parameters. Parameter size: "
                       << GetNumParameter();
        }
        // Set value for model
        if (set_val) {
            set_value();
        }
    }

// Set value for model
    void Model::set_value() {
        // Use distribution to transform the random unsigned
        // int generated by gen into a float in [(0.0, 1.0) * coef]
        std::default_random_engine generator;
        std::uniform_real_distribution<real_t> dis(0.0, 1.0);
        /*********************************************************
         *  Initialize linear and bias term                      *
         *********************************************************/
        for (index_t i = 0; i < param_num_w_; i += aux_size_) {
            param_w_[i] = 0;        /* model */
            for (index_t j = 1; j < aux_size_; ++j) {
                param_w_[i+j] = 1.0;  /* gradient cache */
            }
        }
        param_b_[0] = 0;      /* model */
        for (index_t j = 1; j < aux_size_; ++j) {
            param_b_[j] = 1.0;  /* gradient cache */
        }
        /*********************************************************
         *  Initialize latent factor for fm                      *
         *********************************************************/
        if (score_func_.compare("fm") == 0) {
            index_t k_aligned = get_aligned_k();
            real_t coef = 1.0f / sqrt(num_K_) * scale_;
            real_t* w = param_v_;
            for (index_t j = 0; j < num_feat_; ++j) {
                for(index_t d = 0; d < num_K_; d++, w++)
                    *w = coef * dis(generator);
                for(index_t d = num_K_; d < k_aligned; d++, w++)
                    *w = 0;
                for(index_t d = k_aligned; d < aux_size_*k_aligned; d++, w++)
                    *w = 1.0;
            }
        }
            /*********************************************************
             *  Initialize latent factor for ffm                     *
             *********************************************************/
        else if (score_func_.compare("ffm") == 0) {
            index_t k_aligned = get_aligned_k();
            real_t* w = param_v_;
            real_t coef = 1.0f / sqrt(num_K_) * scale_;
            for (index_t j = 0; j < num_feat_; ++j) {
                for (index_t f = 0; f < num_field_; ++f) {
                    for (index_t d = 0; d < k_aligned; ) {
                        for (index_t s = 0; s < kAlign; s++, w++, d++) {
                            w[0] = (d < num_K_) ? coef * dis(generator) : 0.0;
                            for (index_t j = 1; j < aux_size_; ++j) {
                                w[kAlign * j] = 1.0;
                            }
                        }
                        w += (aux_size_-1) * kAlign;
                    }
                }
            }
        }
    }

// Free the allocated memory
    void Model::free_model() {
        free(param_w_);
        free(param_v_);
        free(param_b_);
        if (param_best_w_ != nullptr) {
            free(param_best_w_);
        }
        if (param_best_v_ != nullptr) {
            free(param_best_v_);
        }
        if (param_best_b_ != nullptr) {
            free(param_best_b_);
        }
    }

// Initialize model from a checkpoint file
    Model::Model(const std::string& filename) {
        CHECK_NE(filename.empty(), true);
        if (this->Deserialize(filename) == false) {
            print_error(
                    StringPrintf("Cannot Load model from the file: %s",
                                 filename.c_str())
            );
            exit(0);
        }
    }

// Serialize current model to a disk file
    void Model::Serialize(const std::string& filename) {
        CHECK_NE(filename.empty(), true);
        FILE* file = OpenFileOrDie(filename.c_str(), "w");
        // Write score function
        WriteStringToFile(file, score_func_);
        // Write loss function
        WriteStringToFile(file, loss_func_);
        // Write feature num
        WriteDataToDisk(file, (char*)&num_feat_, sizeof(num_feat_));
        // Write field num
        WriteDataToDisk(file, (char*)&num_field_, sizeof(num_field_));
        // Write K
        WriteDataToDisk(file, (char*)&num_K_, sizeof(num_K_));
        // Write aux_size
        WriteDataToDisk(file, (char*)&aux_size_, sizeof(aux_size_));
        // Write w
        this->serialize_w_v_b(file);
        Close(file);
    }

// Serialize current model to a txt file.
    void Model::SerializeToTxt(const std::string& filename) {
        CHECK_NE(filename.empty(), true);
        std::ofstream o_file(filename);
        // For now, only LR model can dump to txt file.
        /* bias */
        o_file << (*param_b_) << "\n";
        /* linear term */
        for (index_t n = 0; n < param_num_w_; n+=aux_size_) {
            o_file << *(param_w_+n) << "\n";
        }
        /* letent factor */
        index_t k_aligned = get_aligned_k();
        if (score_func_.compare("fm") == 0) {
            real_t* w = param_v_;
            for (index_t j = 0; j < num_feat_; ++j) {
                for(index_t d = 0; d < num_K_; d++, w++) {
                    o_file << *w;
                    if (d != num_K_-1) {
                        o_file << " ";
                    }
                }
                for(index_t d = num_K_; d < k_aligned; d++, w++) {
                    // do nothing
                }
                for(index_t d = k_aligned; d < aux_size_*k_aligned; d++, w++) {
                    // do nothing
                }
                o_file << "\n";
            }
        } else if (score_func_.compare("ffm") == 0) {
            real_t* w = param_v_;
            for (index_t j = 0; j < num_feat_; ++j) {
                for (index_t f = 0; f < num_field_; ++f) {
                    for (index_t d = 0; d < k_aligned; ) {
                        for (index_t s = 0; s < kAlign; s++, w++, d++) {
                            if (d < num_K_) {
                                o_file << *w;
                                if (d != num_K_-1) {
                                    o_file << " ";
                                }
                            }
                            for (index_t j = 1; j < aux_size_; ++j) {
                                // do nothing
                            }
                        }
                        w += (aux_size_-1) * kAlign;
                    }
                }
                o_file << "\n";
            }
        }
    }

// Deserialize model from a checkpoint file
    bool Model::Deserialize(const std::string& filename) {
        CHECK_NE(filename.empty(), true);
        FILE* file = OpenFileOrDie(filename.c_str(), "r");
        if (file == NULL) { return false; }
        // Read score function
        ReadStringFromFile(file, score_func_);
        // Read loss function
        ReadStringFromFile(file, loss_func_);
        // Read feature num
        ReadDataFromDisk(file, (char*)&num_feat_, sizeof(num_feat_));
        // Read field num
        ReadDataFromDisk(file, (char*)&num_field_, sizeof(num_field_));
        // Read K
        ReadDataFromDisk(file, (char*)&num_K_, sizeof(num_K_));
        // Read aux_size
        ReadDataFromDisk(file, (char*)&aux_size_, sizeof(aux_size_));
        // Read w
        this->deserialize_w_v_b(file);
        Close(file);
        return true;
    }

// Take a record of the best model during training
    void Model::SetBestModel() {
        try {
            if (param_best_w_ == nullptr) {
                param_best_w_ = (real_t*)malloc(
                        param_num_w_*sizeof(real_t));
            }
            if (param_best_v_ == nullptr &&
                score_func_.compare("linear") != 0) {
#ifdef _WIN32
                param_best_v_ = (xLearn::real_t *)_aligned_malloc(
                        param_num_v_ * sizeof(real_t),
                        kAlignByte);
#else
                int ret = posix_memalign(
                (void**)&param_best_v_,
                kAlignByte,
                param_num_v_ * sizeof(real_t));
      CHECK_EQ(ret, 0);
#endif
            }
            if (param_best_b_ == nullptr) {
                param_best_b_ = (real_t*)malloc(
                        aux_size_ * sizeof(real_t)
                );
            }
        } catch (std::bad_alloc&) {
            LOG(FATAL) << "Cannot allocate enough memory for current  \
                   model parameters. Parameter size: "
                       << GetNumParameter();
        }
        // Copy current model parameters
        memcpy(param_best_w_, param_w_, param_num_w_*sizeof(real_t));
        memcpy(param_best_v_, param_v_, param_num_v_*sizeof(real_t));
        memcpy(param_best_b_, param_b_, aux_size_*sizeof(real_t));
    }

// Shrink back for getting the best model
    void Model::Shrink() {
        // Copy best model parameters
        if (param_best_w_ != nullptr) {
            memcpy(param_w_, param_best_w_, param_num_w_*sizeof(real_t));
        }
        if (param_best_v_ != nullptr) {
            memcpy(param_v_, param_best_v_, param_num_v_*sizeof(real_t));
        }
        if (param_best_b_ != nullptr) {
            memcpy(param_b_, param_best_b_, aux_size_*sizeof(real_t));
        }
    }

// Serialize w,v,b to disk file
    void Model::serialize_w_v_b(FILE* file) {
        // Write size of w
        WriteDataToDisk(file, (char*)&param_num_w_, sizeof(param_num_w_));
        // Write size of v
        if (score_func_.compare("linear") != 0) {
            WriteDataToDisk(file, (char*)&param_num_v_, sizeof(param_num_v_));
        }
        // Write w
        WriteDataToDisk(file, (char*)param_w_, sizeof(real_t)*param_num_w_);
        // Write b
        WriteDataToDisk(file, (char*)param_b_, sizeof(real_t)*aux_size_);
        // Write v
        if (score_func_.compare("linear") != 0) {
            WriteDataToDisk(file, (char*)param_v_, sizeof(real_t)*param_num_v_);
        }
    }

// Deserialize w,v,b from disk file
    void Model::deserialize_w_v_b(FILE* file) {
        // Read size of w
        ReadDataFromDisk(file, (char*)&param_num_w_, sizeof(param_num_w_));
        // Read size of v
        if (score_func_.compare("linear") != 0) {
            ReadDataFromDisk(file, (char*)&param_num_v_, sizeof(param_num_v_));
        }
        // Allocate memory. Don't set value here
        this->initial(false);
        // Read w
        ReadDataFromDisk(file, (char*)param_w_, sizeof(real_t)*param_num_w_);
        // Read b
        ReadDataFromDisk(file, (char*)param_b_, sizeof(real_t)*aux_size_);
        // Read v
        if (score_func_.compare("linear") != 0) {
            ReadDataFromDisk(file, (char*)param_v_, sizeof(real_t)*param_num_v_);
        }
    }

}  // namespace xLearn
