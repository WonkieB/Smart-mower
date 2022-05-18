#ifndef MOWER_HPP
#define MOWER_HPP

#include <vector>
#include <memory>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <string.h>

enum Pixel_e {grass, obstacle};
enum Direction_e {up, down, right, left};
typedef std::pair<size_t, size_t> pos_t;

class Pixel{
public:
    Pixel (){}
    virtual ~Pixel() = default;
    
    virtual int visit_pixel() = 0;
    virtual int reset_pixel() = 0;
    virtual Pixel_e get_type() = 0;
};

class Grass : public Pixel{
public:
    Grass(){
        visited = false;
    }

    bool is_visited(){
        return visited;
    }

    virtual int visit_pixel(){
        visited = true;
        return 0;
    }

    virtual int reset_pixel(){
        visited = false;
        return 0;
    }

    virtual Pixel_e get_type(){
        return grass;
    }

private:
    bool visited;

};

class Obstacle : public Pixel{
public:
    virtual int visit_pixel(){
        return 1;
    }
    virtual int reset_pixel(){
        return 0;
    }
    virtual Pixel_e get_type(){
        return obstacle;
    }

};

class Lawn{
public:
    Lawn(std::string map_file){
        std::vector<std::shared_ptr<Pixel>> temp_row;
        std::ifstream file(map_file);
        std::string line;
        std::stringstream str_stream;
        std::string map_char;
        while(getline(file, line)){
            
            std::cout<<line<<std::endl;
            if(line.front() != '#'){
                std::stringstream().swap(str_stream); // clear stringstream
                map_char.erase();
                temp_row.clear();
                str_stream << line;
                while (std::getline(str_stream, map_char, ' ')){
                    if ('g' == map_char.front()){
                        auto point = std::make_shared<Grass>();
                        temp_row.push_back(point);
                    } 
                    else if ('O' == map_char.front()){
                        auto point = std::make_shared<Obstacle>();
                        temp_row.push_back(point);
                    } 
                    else if ('S' == map_char.front()){
                        auto point = std::make_shared<Grass>();
                        point->visit_pixel();
                        temp_row.push_back(point);
                        mower_pos = std::make_pair(pixel_arr.size(), temp_row.size()-1);
                    } 
                    else if (' ' != map_char.front() && '\n' != map_char.front() && '\0' != map_char.front() && '\r' != map_char.front()){
                        std::cout << "Unrecognized character" << std::endl;
                    }
                }

                pixel_arr.push_back(temp_row);
            }
        }
    }

    int set_pixel_type(pos_t pos, Pixel_e type);
    int clear_visited();
    unsigned int count_unvisited(){
        unsigned int unvisited = 0;
        for (auto row : pixel_arr){
            for (auto pixel : row){
                if (grass == pixel->get_type()){
                    if(!std::dynamic_pointer_cast<Grass>(pixel)->is_visited()){
                        ++unvisited;
                    }
                } 
            }
        }
        return unvisited;
    }
    int reset_map();
    void display_map(){
        std::cout << std::endl;
        for (size_t i = 0; i < pixel_arr.size(); ++i){
            for (size_t j = 0; j < pixel_arr[i].size(); ++j){
                if (obstacle == pixel_arr[i][j]->get_type()){
                    std::cout << "O ";
                }
                else if (grass == pixel_arr[i][j]->get_type()){
                    if (std::make_pair(i,j) == mower_pos){
                        std::cout << "M ";
                    } 
                    else if(!std::dynamic_pointer_cast<Grass>(pixel_arr[i][j])->is_visited()){
                        std::cout << "| ";
                    }
                    else{
                        std::cout << ". ";
                    }
                }
                
            }
            std::cout << std::endl;
        }
    }
    int move_mower(Direction_e dir){
        pos_t new_pos = mower_pos;
        int result = 1;
        switch(dir){
            case right:
                if(mower_pos.second < pixel_arr[0].size()){
                    new_pos.second += 1;
                    result = pixel_arr[new_pos.first][new_pos.second]->visit_pixel();
                }
                break;

            case left:
                if(mower_pos.second > 0){
                    new_pos.second -= 1;
                    result = pixel_arr[new_pos.first][new_pos.second]->visit_pixel();
                }
                
                break;
            
            case down:
                if(mower_pos.first > 0){
                    new_pos.first += 1;
                    result = pixel_arr[new_pos.first][new_pos.second]->visit_pixel();
                }
                break;
            
            case up:
                if(mower_pos.first < pixel_arr.size()){
                    new_pos.first -= 1;
                    result = pixel_arr[new_pos.first][new_pos.second]->visit_pixel();
                }
                break;
            
            default:
                break;
        }

        if(0 == result){ // move successful
            mower_pos = new_pos;
            display_map();
        }
        return result;
    }

private:

    pos_t mower_pos;
    std::vector<std::vector<std::shared_ptr<Pixel>>> pixel_arr;
};


#endif // MOWER_HPP