#include <memory>
#include <iostream>
#include <string>
#include <vector>

#include <cpprest/http_client.h>

// using namespace web;
// using namespace utility;
// using namespace http;
// using namespace http::client;

#define USER_ID             U("userId")
#define ID                  U("id")
#define TITLE               U("title")
#define BODY                U("body")
#define EMAIL               U("email")
#define POST_ID             U("postId")
#define NAME                U("name")
#define COMMENTS            U("comments")

struct Comment
{
    unsigned int post_id{};
    unsigned int id{};
    std::string name{};
    std::string email{};
    std::string body{};

    Comment(){};
    Comment(const unsigned int poist_id, const unsigned int id, const std::string & name, const std::string & email, const std::string & body)
    : post_id(post_id), id(id), name(name), email(email), body(body)
    { }

    static Comment FromJSON(const web::json::object & obj)
    {
        unsigned int post_id = (unsigned int) obj.at(POST_ID).as_integer();
        unsigned int id = (unsigned) obj.at(ID).as_integer();
        std::string name = obj.at(NAME).as_string();
        std::string email = obj.at(EMAIL).as_string();
        std::string body = obj.at(BODY).as_string();
    }

    web::json::value AsJSON()
    {
        web::json::value result = web::json::value::object();
        result[POST_ID] = web::json::value::number(post_id);
        result[ID] = web::json::value::number(id);
        result[NAME] = web::json::value::string(name);
        result[EMAIL] = web::json::value::string(email);
        result[BODY] = web::json::value::string(body);
        return result;
    }
};

struct Post
{
    unsigned int user_id;
    unsigned int id;
    std::string title;
    std::string body;
    std::vector<Comment> comments;

    Post(){}
    Post(const unsigned int id) : id(id) {}

    Post (const unsigned int user_id, const std::string & t, const std::string & b)
    : user_id(user_id), title(t), body(b)
    {}

    Post (const unsigned int user_id, const unsigned int id, const std::string & t, const std::string & b)
    : user_id(user_id), title(t), body(b), id(id)
    {}

    Post (const unsigned int user_id, const unsigned int id, const std::string & t, const std::string & b, const std::vector<Comment> & comments) 
    : user_id(user_id), title(t), body(b), id(id), comments(comments)
    {}

    static Post FromJSON(const web::json::object & object)
    {
        if (object.size() == 1)
        {
            unsigned int id = (unsigned int) object.at(ID).as_integer(); 
            return Post(id);
        }

        // If key is not found it throws web::json::json_exception
        unsigned int user_id = (unsigned int) object.at(USER_ID).as_integer(); 
        unsigned int id = (unsigned int) object.at(ID).as_integer(); 
        std::string title = object.at(TITLE).as_string(); 
        std::string body = object.at(BODY).as_string(); 
        std::vector<Comment> comments; 
            
        bool hasComments = object.find(COMMENTS) != object.end();
        if (hasComments)
        {
            auto commBlock = object.at(COMMENTS);
            for (auto it = commBlock.as_array().begin(); it != commBlock.as_array().end(); it++)
            {
                Comment comment = Comment::FromJSON(it->as_object());
                comments.push_back(comment);
            }

            return Post(user_id, id, title, body, comments);
        }
        else
        {
            return Post(user_id, id, title, body);
        }

        // alternative
        // auto it_id = object.find(ID);
        // if (it_id == object.end())
        // {
        //     throw web::json::json_exception(U("id key not found"));
        // }
    }

    web::json::value AsJSON() const
    {
        web::json::value result = web::json::value::object();
        result[USER_ID] = web::json::value::number(this->user_id);
        result[ID] = web::json::value::number(this->id);
        result[TITLE] = web::json::value::string(this->title);
        result[BODY] = web::json::value::string(this->body);
        return result;
    }

    friend std::ostream & operator<< (std::ostream & out, const Post & post)
    {
        out << "Post " << post.id << ", Title: " << post.title;
        return out;
    }
};

bool CheckResponse (const web::http::http_response & response )
{
    if (response.status_code() < 300)
        std::cout << "Request was successful" << std::endl;
    else
    {
        std::cout << "Request failed with " << response.status_code() << std::endl;
        std::cout << "---------------------------------" << std::endl;
        std::cout << response.to_string() << std::endl;
        std::cout << "---------------------------------" << std::endl;
        return false;
    }
    return true;
}

void GetPost(const web::http::http_response & response)
{
    if (!CheckResponse(response))
        return;
    try
    {
        web::json::value json = response.extract_json().get();
        if (json.is_array())
        {
            std::cout << "Total number of posts : " << json.size() << std::endl;
            for (auto it = json.as_array().begin(); it != json.as_array().end(); it++)
            {
                Post post = Post::FromJSON(it->as_object());
                std::cout << post << std::endl;
            }
        }
        else
        {
            Post post = Post::FromJSON(json.as_object());
            std::cout << post << std::endl;
        }
    }
    catch (const web::json::json_exception & e)
    {
        std::cout << "FAILED: " << e.what() << std::endl;
    }
}


// The C++ main function - the place where any adventure starts
int main() 
{
    std::cout << "Application started ... " << std::endl;

    // create client
    utility::string_t address = U("http://jsonplaceholder.typicode.com/posts/");
    web::http::uri uri = web::http::uri(address);
    web::http::client::http_client post_client(web::http::uri_builder(uri).to_uri());

    // GET one element
    web::uri_builder get_builder(U("/2"));
    auto get_request = post_client.request(web::http::methods::GET, get_builder.to_string());
    std::cout << "---------------------------------" << std::endl;
    std::cout << "GET Request send ... " << std::endl;
    GetPost(get_request.get());
    

    // POST one element
    Post my_new_post = Post(10, "My New Value", "This is the body");
    auto post_request = post_client.request(web::http::methods::POST);
    std::cout << "---------------------------------" << std::endl;
    std::cout << "POST Request send ... " << std::endl;
    GetPost(post_request.get());

    // GET all elements
    auto get_all_request = post_client.request(web::http::methods::GET);
    std::cout << "---------------------------------" << std::endl;
    std::cout << "GET ALL Request sent ..." << std::endl;
    auto resp = get_all_request.get();
    GetPost(resp);


}
