/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __GEMS_SCENE_H__
#define __GEMS_SCENE_H__

#include "cocos2d.h"

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

const int GRID_SIZE = 9;

struct Cell
{
    cocos2d::Sprite* _sprite;
    unsigned int _type;
    unsigned _count;
};

class Gems : public cocos2d::Scene
{
public:

    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
    // implement the "static create()" method manually
    CREATE_FUNC(Gems);

protected:
    cocos2d::Vec2 getGemPosition(int CellX, int CellY);
    bool fillCell(int CellX, int CellY);
    void processMove(bool UseAnimations);

    void clearGridCounts();
    int markSameColorGems(int CellX, int CellY);
    int markSameColorGemsHorz(int CellX, int CellY, unsigned Type, bool HorzOnly, int& Left, int& Right);
    int markSameColorGemsVert(int CellX, int CellY, unsigned Type, bool VertOnly, int& Bottom, int& Top);
    void removeMarked(bool UseAnimations);
    
    void onExchangeComplete(float delta);
    void onMoveComplete(float delta);
    void onUpdateScores(float delta);

//     int _gemsCount;
    cocos2d::Texture2D* _gems;
    bool _ignoreInput;
    int _score, _screenScore;
    cocos2d::Label* _labelScore;
    int _cellFromX, _cellFromY, _cellToX, _cellToY;
    cocos2d::Sprite* _board;
//     cocos2d::Texture2D* _gems;
    Cell _grid[GRID_SIZE][GRID_SIZE];
};

#endif // __GEMS_SCENE_H__
