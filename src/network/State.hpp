#pragma once

#include <functional>
#include <map>
#include <vector>

enum class State : int
{
    //Close = 0,
    Connect,
    Reconnect,
    Disconnect,
    Send,
    Receive,
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
            //{Strategy::Continue, {Trigger::SendBuffer, State::Send}},
            //{Strategy::Reconnect, {Trigger::CloseConnection, State::Close}},
        };

        rules[State::Reconnect] = {
            {Strategy::Continue ,{Trigger::CloseConnection, State::Connect}},
            //{Strategy::Continue ,{Trigger::ConnectHost, State::Connect}},
        };

        rules[State::Disconnect] = {
            {Strategy::Continue, {Trigger::CloseConnection, State::Idle}},
            //{Strategy::Continue, {Trigger::StopTransmission, State::Stop}},
        };

        rules[State::Send] = {
            {Strategy::Continue, {Trigger::SendBuffer, State::Receive}},
            //{Strategy::Continue, {Trigger::ReceiveBuffer, State::Receive}},
            //{Strategy::Reconnect, {Trigger::CloseConnection, State::Close}},
            //{Strategy::Repeat, {Trigger::SendBuffer, State::Send}},
            //{Strategy::Disconnect, {Trigger::StopTransmission, State::Stop}},
        };

        rules[State::Receive] = {
            {Strategy::Continue, {Trigger::ReceiveBuffer, State::Send}},
            {Strategy::Repeat, {Trigger::ReceiveBuffer, State::Receive}},
            {Strategy::Reconnect, {Trigger::CloseConnection, State::Connect}},
            {Strategy::Disconnect, {Trigger::CloseConnection, State::Disconnect}},
            {Strategy::Listen, {Trigger::IdleTransmission, State::Idle}},
            //{Strategy::Continue, {Trigger::SendBuffer, State::Send}},
            //{Strategy::Reconnect, {Trigger::CloseConnection, State::Close}},
            //{Strategy::Repeat, {Trigger::ReceiveBuffer, State::Receive}},
            //{Strategy::Disconnect, {Trigger::StopTransmission, State::Stop}},
        };

        rules[State::Idle] = {
            {Strategy::Continue, {Trigger::IdleTransmission, State::Idle}},
            //{Strategy::Continue, {Trigger::StopTransmission, State::Stop}},
        };
    }

    void addTrigger(Trigger trigger, std::function<std::unique_ptr<IStrategy>()> event)
    {
        events[trigger] = event;
    }

    std::unique_ptr<IStrategy> changeState()
    {
        auto response = events[rules[currentState][Strategy::Continue].first]();
        //currentState = rules[currentState][Strategy::Continue].second;
        currentState = rules[currentState][response->policy()].second;
        /*if (response->origin() == Origin::Erro)
        {
            events[rules[currentState][1].first]();
        }*/
        return response;
    }

private:
    State currentState{ State::Connect };

    std::map<State, std::map<Strategy, std::pair<Trigger, State>>> rules;
    std::map<Trigger, std::function<std::unique_ptr<IStrategy>()>> events;
};