//
// Created by Khurram Javed on 2022-07-19.
//

#include "../../include/environments/proto_prediction_environments.h"
#include <fstream>
#include <iostream>

ProtoPredictionEnvironment::ProtoPredictionEnvironment(std::string game_name,
                                                       float gamma)
    : gray_features(210 * 160, 0), observation(84 * 84 * 16 + 20, 0)
{
    //    std::cout << "Initializing env\n";\\

    std::cout << "../policies/" + game_name + "NoFrameskip-v4.txt" << std::endl;
    std::fstream* policy = new std::fstream("../policies/" + game_name + "NoFrameskip-v4.txt",
                                            std::ios::in | std::ios::binary);
    //    std::cout << "Policy loaded\n";
    my_env = new ale::ALEInterface();
    this->gamma = gamma;

    my_env->setInt("random_seed", 1731038949);
    //  my_env.setBool("truncate_on_loss_of_life", true);
    my_env->setFloat("repeat_action_probability", 0.0);
    my_env->setInt("frame_skip", 1);
    //    std::cout << "Int set\n";
    my_env->loadROM("../games/" + game_name + ".bin");
    //    std::cout << "Bin loaded\n";
    my_env->reset_game();
    long size;
    policy->seekg(0, std::ios::end);
    size = policy->tellg();
    std::cout << "Policy size " << size << std::endl;
    policy->seekg(0, std::ios::beg);
    actions = new char[size];
    policy->read(actions, size);
    policy->close();
    delete policy;
    action_set = my_env->getMinimalActionSet();
    time = 0;
    reward = 0;
    ep_reward = 0;
    to_reset = false;
    std::cout << "Constructor done\n";
    this->last_action = 0;
}

ProtoPredictionEnvironment::~ProtoPredictionEnvironment()
{
    delete my_env;
}


std::vector<float> ProtoPredictionEnvironment::get_state()
{
    return {};
}

void ProtoPredictionEnvironment::UpdateReturns()
{
    float old_val = 0;
    list_of_returns = std::vector<float>(list_of_rewards.size(), 0);
    for (int i = list_of_rewards.size() - 1; i >= 0; i--)
    {
        list_of_returns[i] = list_of_rewards[i] + old_val;
        old_val = list_of_returns[i] * this->gamma;
    }
    list_of_rewards.clear();
}

// S, 1, S, 1, S, R,
std::vector<float>& ProtoPredictionEnvironment::GetListOfReturns()
{
    return this->list_of_returns;
}

bool ProtoPredictionEnvironment::get_done() { return true; }

std::vector<float> FastStep()
{
    return {};
}

std::vector<float> ProtoPredictionEnvironment::step()
{
    to_reset = false;
    time++;
    if (actions[time] == 'R')
    {
        reward = 0;
        my_env->reset_game();
        to_reset = true;
    }
    else
    {
        this->last_action = int(actions[time]) - 97;
        reward = my_env->act(action_set[int(actions[time]) - 97]);
    }
    return this->get_state();
}

float ProtoPredictionEnvironment::TakeAction(int action)
{
    this->last_action = current_action;
    current_action = action;
    time++;
    my_env->getMinimalActionSet();
    if (action == 0)
    {
        //        std::cout << "Taking action left \n";
        reward = my_env->act(ale::PLAYER_A_LEFT);
    }
    else if (action == 1)
    {
        //        std::cout << "Taking action right \n";
        reward = my_env->act(ale::PLAYER_A_RIGHT);
    }
    else if (action == 2)
    {
        reward = my_env->act(ale::PLAYER_A_NOOP);
    }

    //    std::cout << reward << std::endl;

    if (my_env->game_over())
    {
        my_env->reset_game();
    }
    if (reward > 0.5)
    {
        return 1;
    }
    else if (reward < -0.5)
    {
        return -1;
    }
    return 0;
}


std::vector<float> ProtoPredictionEnvironment::FastStep()
{
    to_reset = false;
    time++;

    if (actions[time] == 'R')
    {
        my_env->reset_game();
        to_reset = true;
    }
    else
    {
        reward = my_env->act(action_set[int(actions[time]) - 97]);
    }

    return {};
}

float ProtoPredictionEnvironment::get_target() { return real_target[time]; }

float ProtoPredictionEnvironment::get_gamma()
{
    //    if (to_reset)
    //        return 0;
    return this->gamma;
}

float ProtoPredictionEnvironment::get_reward()
{
    if (reward > 0.5)
        return 1;
    else if (reward < -0.5)
        return -1;
    return 0;
}
