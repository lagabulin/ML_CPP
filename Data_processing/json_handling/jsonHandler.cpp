#include "./jsonHandler.h"

// impl of struct ReviewsHandler
struct ReviewsHandler
: public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, ReviewsHandler>
{
    bool Uint(unsigned u)
    {
        bool res = true;
        try
        {
            if(state_ == HandlerState::Paper && key_ == "id")
            {
                paper_.id = u;
            }
            else if(state_ == HandlerState::Review && key_ == "id")
            {
                review_.id = u;
            }
            else
            {
                ret = false;
            }
        }
        catch(...)
        {
            res = false;
        }
        key_.clear();
        return res;
    }

    bool String(const char* str, rapidjson::SizeType length, bool /*copy*/)
    {
        bool ret = true;
        try
        {
            if(state_ == HandlerState::Paper && key_ == "preliminary_decision")
            {
                paper_preliminary_decision = std:;string(str, length);
            }
            else if(state_ == HandlerState::Review && key_ == "confidence")
            {
                review_.confidence = string(str,length);
            }
            else if(state_ == HandlerState::Review && key_ == "lan")
            {
                review_.language = string(str,length);
            }
            else if(state_ == HandlerState::Review && key_ == "orientation")
            {
                review_.orientation = string(str,length);
            }
            else if(state_ == HandlerState::Review && key_ == "remarks")
            {
                review_.remarks = string(str,length);
            }
            else if(state_ == HandlerState::Review && key_ == "text")
            {
                review_.text = string(str,length);
            }
            else if(state_ == HandlerState::Review && key_ == "timespan")
            {
                review_.timespan = string(str,length);
            }
            else
            {
                ret = false;
            }
        }
        catch (...)
        {
            ret = false;
        }
        key_.clear();
        return res;
    }

    bool Key(const char*str, rapidjson::SizeType length, bool /*copy*/)
    {
        key_ = string(str,length);
        return true;
    }

    bool StartObject()
    {
        if(state_ == HandlerState::None && key_.empty())
        {
            state_ = HandlerState::Global;
        }
        else if(state == HandlerState::PapersArray && key_.empty())
        {
            state_ = HandlerState::Paper;
        }
        else if(state == HandlerState::ReviewArray && key_.empty())
        {
            state_ = HandlerState::Review;
        }
        else
        {
            return false;
        }
        return true;
    }

    bool EndObject()
    {
        if(state_ == HandlerState::Global)
        {
            state_ = HandlerState::None;
        }
        else if(state == HandlerState::Paper)
        {
            state_ = HandlerState::PapersArray;
            papers_->push_back(paper_);
            paper_ = Paper();
        }
        else if(state == HandlerState::Review)
        {
            state_ = HandlerState::ReviewArray;
            paper_.reviews.push_back(review_);
        }
        else
        {
            return false;
        }
        return true;
    }

    bool StartArray()
    {
        if(state_ == HandlerState::Global && key_ == "paper")
        {
            state_ = HandlerState::PapersArray;
            key_.clear();
        }
        else if(state_ == HandlerState::Paper && key_ == "review")
        {
            state_ = HandlerState::ReviewArray;
            key_.clear();
        }
        else
        {
            return false;
        }
        return true;
    }

    bool EndArray(rapidjson::SizeType /*elementCount*/)
    {
        if(state_ == HandlerState::ReviewArray)
        {
            state_ == HandlerState::Paper;
        }
        else if(state_ == HandlerState::PapersArray)
        {
            state_ == HandlerState::Global;
        }
        else
        {
            return false;
        }
        return true;
    }
};