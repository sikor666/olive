#pragma once

#include <functional>
#include <map>
#include <vector>

enum class State : int
{
    Close = 0,
    Connect,
    Send,
    Receive,
};

enum class Trigger : int
{
    CloseConnection,
    ConnectHost,
    SendBuffer,
    ReceiveBuffer,
};

class StateMachine
{
public:
    StateMachine()
    {
        rules[State::Close] = {
            {Trigger::ConnectHost, State::Connect}
        };

        rules[State::Connect] = {
            {Trigger::SendBuffer, State::Send},
            {Trigger::CloseConnection, State::Close}
        };

        rules[State::Send] = {
            {Trigger::ReceiveBuffer, State::Receive},
            {Trigger::CloseConnection, State::Close}
        };

        rules[State::Receive] = {
            {Trigger::SendBuffer, State::Send},
            {Trigger::CloseConnection, State::Close}
        };
    }

    void addTrigger(Trigger trigger, std::function<std::unique_ptr<IResponse>()> event)
    {
        events[trigger] = event;
    }

    std::unique_ptr<IResponse> changeState()
    {
        auto input = 0;
        auto response = events[rules[currentState][input].first]();
        if (response->origin() == Origin::Erro)
        {
            events[rules[currentState][++input].first]();
        }
        currentState = rules[currentState][input].second;
        return response;
    }

private:
    State currentState{ State::Close }, exitState{ State::Close };

    std::map<State, std::vector<std::pair<Trigger, State>>> rules;
    std::map<Trigger, std::function<std::unique_ptr<IResponse>()>> events;
};