/***********************************************************************************
 * Copyright(c) 2017 by Samsung Techwin, Inc.
 *
 * This software is copyrighted by, and is the sole property of Samsung Techwin.
 * All rights, title, ownership, or other interests in the software remain the
 * property of Samsung Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Samsung Techwin.
 *
 * Samsung Techwin reserves the right to modify this software without notice.
 *
 * Samsung Techwin, Inc.
 * KOREA
 * http://www.samsungtechwin.co.kr
 *********************************************************************************/

/**
 * @file VideoManager.h
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * VideoManager.h
 *
 *  Created on: 2017. 3. 31.
 *      Author: choi.sungwoon
 */

#ifndef VIDEOMANAGER_H_
#define VIDEOMANAGER_H_

#include "Serializable.h"


/**
 *
 * @class 
 * @brief 
 */
class VideoManager: public Serializable
{
private:
    int att1;

/**
 *
 * @class 
 * @brief 
 */
    class VideoProfile: public Serializable
    {
    private:
        int att3;
        float att4;
        string att5;
        int a[2];
    public:

/**
 *
 */
        VideoProfile() : Serializable(), att3(5), att4(2.3), att5()
        {
            // default
            this->a[0] = 88;
            this->a[1] = 99;
        }

/**
 *
 * @return 
 */
        virtual ~VideoProfile()
        {
        }


/**
 *
 */
        void write()
        {
            this->set_write(this->att3, "att30");
            this->set_write(this->att4, "att40");
            this->set_write(this->a, 2, "a");
            this->set_write(this->att5, "att50");
        }

/**
 *
 */
        void read()
        {
            this->get_read(this->att3, "att30");
            this->get_read(this->att4, "att40");
            this->get_read(this->att5, "att50");
            this->get_read(this->a, 2, "a");
        }
    };


    VideoProfile videoProfile[2];
    float att2;
    int array[2];

public:

/**
 *
 */
    VideoManager()
    {
        this->att1 = 5;
        this->att2 = 2.3;

        this->array[0] = 333;
        this->array[1] = 444;
    }

/**
 *
 * @return 
 */
    virtual ~VideoManager()
    {
    }


/**
 *
 */
    void read()
    {
        this->get_read(this->array, 2, "arrayA");
        this->get_read(this->att1, "att10");
        this->get_read(this->videoProfile, 2, "videoProfile");
        this->get_read(this->att2, "att20");
    }

/**
 *
 */
    void write()
    {
        this->set_write(this->att1, "att10");
        this->set_write(this->videoProfile, 2, "videoProfile");
        this->set_write(this->att2, "att20");
        this->set_write(this->array, 2, "arrayA");
    }
};


/**
 *
 * @class 
 * @brief 
 */
class A: public Serializable
{
private:
    string b[3];

/**
 *
 * @class 
 * @brief 
 */
    class F : public Serializable
    {
        string g;
        string i;

/**
 *
 */
        void read()
        {
            this->get_read(g, "g");
            this->get_read(i, "i");
        }

/**
 *
 */
        void write()
        {
            this->set_write(g, "g");
            this->set_write(i, "i");
        }
    };
    F f;
    string d;


/**
 *
 */
    void read()
    {
        this->get_read(this->b, 3, "b");
        this->get_read(this->d, "d");
        this->get_read(f, "f");
    }


/**
 *
 */
    void write()
    {
        this->set_write(this->b, 3, "b");
        this->set_write(this->d, "d");
        this->set_write(f, "f");
    }
};

#endif /* VIDEOMANAGER_H_ */
