//
// Created by Khurram Javed on 2024-02-18.
//

#include <iostream>

#include <vector>
#include <fstream>
#include "include/swifttd/SwiftTD.h"
#include "include/environments/proto_prediction_environments.h"
#include <random>
#include "include/experiment/Experiment.h"
#include "src/atari/ale_interface.hpp"
#include "include/evaluators/predictionEvaluator.h"
#include "include/logger/logger.h"
#include "include/json.hpp"


std::vector<unsigned char> observation(400000, 0);
std::vector<unsigned char> resized_observation;
std::vector<int> active_feature_indices;


void preprocess_feautres(std::vector<unsigned char>& observation, std::vector<unsigned char>& resized_observation)
{
    resized_observation.clear();
    for (int row = 0; row < 210; row += 2)
    {
        for (int col = 0; col < 160; col += 2)
        {
            for (int channel = 0; channel < 1; channel++)
            {
                int idx = row * 160 * 1 + col * 1 + channel;
                int temp =
                (observation[idx] + observation[idx + 3] + observation[idx + 160 * 3] + observation[idx + 160 * 3 +
                    3]) / 4;
                resized_observation.push_back(temp);
            }
        }
    }
}

void binarize_features(std::vector<unsigned char>& resized_observation,
                       std::vector<int>& active_feature_indices)
{
    active_feature_indices.clear();
    for (int i = 0; i < resized_observation.size(); i++)
    {
        int val = int(resized_observation[i] / 8);
        active_feature_indices.push_back(i * 32 + val);
    }
}


Experiment* my_experiment;

int main(int argc, char* argv[])
{
    my_experiment = new ExperimentJSON(argc, argv);


    std::cout << "Starting experiment\n";
    SwiftTDSparse* t = new SwiftTDSparse(400000, my_experiment->get_float_param("lambda"),
                                         my_experiment->get_float_param("alpha"),
                                         my_experiment->get_float_param("gamma"),
                                         my_experiment->get_float_param("epsilon"),
                                         my_experiment->get_float_param("meta-step-size"),
                                         my_experiment->get_float_param("eta"),
                                         my_experiment->get_float_param("decay"));

    ProtoPredictionEnvironment* env2 = new ProtoPredictionEnvironment(my_experiment->get_string_param("env"),
                                                                      my_experiment->get_float_param("gamma"));


    PredictionEvaluator evaluator(my_experiment->get_float_param("gamma"));

    FileLogger data_logger(my_experiment->get_string_param("name") + "_" + std::to_string(my_experiment->run) + ".json");


    nlohmann::json j;
    for (auto const& x : my_experiment->args_for_run)
    {
        j[x.first] = x.second;
    }


    std::cout << "Learner init\n";


    std::vector<float> list_of_predictions;
    std::vector<float> list_of_rewards;
    std::vector<float> steps;

    float error = 0;
    float total = 0;
    for (int i = 0; i < my_experiment->get_int_param("steps"); i++)
    {
        env2->step();
        env2->my_env->getScreenGrayscale(observation);
        preprocess_feautres(observation, resized_observation);
        binarize_features(resized_observation, active_feature_indices);
        // add reward as a feature
        if (env2->get_reward() == 1)
        {
            active_feature_indices.push_back(300000);
        }

        else if (env2->get_reward() == -1)
        {
            active_feature_indices.push_back(300001);
        }
        else
        {
            active_feature_indices.push_back(300002);
        }
        // add past action as a feature
        active_feature_indices.push_back(env2->last_action + 300003);

        float val = t->Step(active_feature_indices, env2->get_reward());
        evaluator.addPredictionAndReward(val, env2->get_reward());
        list_of_predictions.push_back(val);
        list_of_rewards.push_back(env2->get_reward());
        steps.push_back(i);
        total++;
        if (i % 5000 == 0)
        {
            std::cout << "Step: " << i << " / " << my_experiment->get_int_param("steps") << std::endl;
        }
    }

    j["Lifetime_error"] = evaluator.getMSE(total);
    j["Predictions_in_final_2k_steps"] = evaluator.getPredictions(total - 500, total);
    j["Returns_in_final_2k_steps"] = evaluator.getReturns(total - 500, total);
    // convert j to string and log it with the data logger
    data_logger.log(j.dump());
    return 0;
}
