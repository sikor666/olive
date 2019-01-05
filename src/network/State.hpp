#pragma once

#include "Strategy.hpp"

#include <functional>
#include <map>
#include <vector>

enum class State : int
{
    //Reconnect,
    Connect,
    Send,
    Receive,
    Disconnect,
    Idle,
};

enum class Trigger : int
{
    ConnectHost,
    SendBuffer,
    ReceiveBuffer,
    CloseConnection,
    IdleTransmission,
};

class StateMachine
{
public:
    StateMachine()
    {
        rules[State::Connect] = {
            {Strategy::Continue, {Trigger::ConnectHost, State::Send}},
        };

        /*rules[State::Reconnect] = {
            {Strategy::Continue ,{Trigger::CloseConnection, State::Connect}},
        };*/

        rules[State::Disconnect] = {
            {Strategy::Continue, {Trigger::CloseConnection, State::Idle}},
        };

        rules[State::Send] = {
            {Strategy::Continue, {Trigger::SendBuffer, State::Receive}},
        };

        rules[State::Receive] = {
            {Strategy::Continue, {Trigger::ReceiveBuffer, State::Send}},
            {Strategy::Repeat, {Trigger::ReceiveBuffer, State::Receive}},
            //{Strategy::Reconnect, {Trigger::CloseConnection, State::Connect}},
            {Strategy::Disconnect, {Trigger::CloseConnection, State::Disconnect}},
            {Strategy::Listen, {Trigger::IdleTransmission, State::Idle}},
        };

        rules[State::Idle] = {
            {Strategy::Continue, {Trigger::IdleTransmission, State::Idle}},
        };
    }

    void addTrigger(Trigger trigger, std::function<std::unique_ptr<IStrategy>()> event)
    {
        events[trigger] = event;
    }

    void changeState()
    {
        auto strategy = events[rules[currentState][Strategy::Continue].first]();
        currentState = rules[currentState][strategy->policy()].second;
    }

private:
    State currentState{ State::Connect };

    std::map<State, std::map<Strategy, std::pair<Trigger, State>>> rules;
    std::map<Trigger, std::function<std::unique_ptr<IStrategy>()>> events;
};