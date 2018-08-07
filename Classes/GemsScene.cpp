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

#include "GemsScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

const float ANIMATION_DURATION = 0.5f;
const float UPDATE_SCORES_DELTA = 0.05f;
const float SELECTED_SCALE = 1.1f;
const int SCORE_LABELS_SIZE = 16;
const int LABEL_SPEED = 200;
// for 0, 1, 2, 3, 4, 5, 6, 7 gems in row
const unsigned GEM_SCORE[] = { 0, 0, 0, 15, 20, 30, 50, 100, 250, 500, 1000 };

static int Min(int V1, int V2) { return (V1 < V2) ? V1 : V2; }
static int Max(int V1, int V2) { return (V1 > V2) ? V1 : V2; }

Scene* Gems::createScene()
{
    return Gems::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in GemsScene.cpp\n");
}

// on "init" you need to initialize your instance
bool Gems::init()
{
    //_gemsCount = 0;
    _ignoreInput = false;
    _score = _screenScore = 0;
    _cellFromX = _cellFromY = -1;
    _cellToX = _cellToY = -1;
    // init scene
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a menu item, which is clicked to quit the program

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create("CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(Gems::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
//         float y = origin.y + closeItem->getContentSize().height/2;
        float y = origin.y + visibleSize.height - closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // add "board" sprite
    _board = Sprite::create("Board.png");
    if (_board == nullptr)
    {
        problemLoading("'Board.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        _board->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add touch handler
        auto touchListener = EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = CC_CALLBACK_2(Gems::onTouchBegan, this);
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->addEventListenerWithSceneGraphPriority(touchListener, _board);

        // add the sprite as a child to this
        this->addChild(_board, 0);
    }
    // add border to board
    auto boardBorders = Sprite::create("BoardBorders.png");
    if (boardBorders == nullptr)
    {
        problemLoading("'BoardBorders.png'");
    }
    else
    {
        auto size = _board->getContentSize();
        boardBorders->setPosition(Vec2(size.width / 2, size.height / 2));
        _board->addChild(boardBorders, 1);
    }

    // create and initialize a label
    _labelScore = Label::createWithTTF("Score: 0", "fonts/Marker Felt.ttf", 24);
    if (_labelScore == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // origin.y + (visibleSize.height + boardBorders->getContentSize().height + _labelScore->getContentSize().height) / 2
        // position the label on the top center of the screen
        _labelScore->setPosition(Vec2(origin.x + 0.5f * visibleSize.width,
                                origin.y + 0.5f * (visibleSize.height + boardBorders->getContentSize().height + _labelScore->getContentSize().height)));
//                                 origin.y + visibleSize.height - _labelScore->getContentSize().height / 2));

        // add the label as a child to this layer
        this->addChild(_labelScore, 2);
    }

    _gems = Director::getInstance()->getTextureCache()->addImage("Gems.png");
    
//     auto spritecache = SpriteFrameCache::getInstance();
//     spritecache->addSpriteFramesWithFile("Gems.plist");
//     
//     for (int i = 0; ; i++)
//     {
//         char spriteFrameName[32];
//         sprintf(spriteFrameName, "Gems_%.2d.png", i);
//         SpriteFrame* sf = spritecache->getSpriteFrameByName(spriteFrameName);
//         if (nullptr == sf)
//         {
//             _gemsCount = i;
//             break;
//         }
//     }

    // generate gems grid
    for (unsigned j = 0; j < ARRAYSIZE(_grid); j++)
    {
        for (unsigned i = 0; i < ARRAYSIZE(_grid[j]); i++)
        {
            fillCell(i, j);
            if (nullptr != _grid[j][i]._sprite)
            {
                _grid[j][i]._sprite->setPosition(getGemPosition(i, j));
            }
        }
    }
    // make field stable
    int count;
    do
    {
        count = 0;
        for (int j = 0; j < GRID_SIZE; j++)
        {
            for (int i = 0; i < GRID_SIZE; i++)
            {
                if (0 == _grid[j][i]._count)
                {
                    count += markSameColorGems(i, j);
                }
            }
        }
        removeMarked(false);
    }
    while (count > 0);
    
    return true;
}

bool Gems::fillCell(int CellX, int CellY)
{
    // get texture size
    auto gemsSize = _gems->getContentSize();
    int gemsCount = int(gemsSize.width / gemsSize.height);
    // random gem index
    int gemIndex = RandomHelper::random_int(0, gemsCount-1);
    Rect rect(gemIndex*gemsSize.height, 0, gemsSize.height, gemsSize.height);
    auto sprite = Sprite::createWithTexture(_gems, rect, false);
//     char spriteFrameName[32];
//     sprintf(spriteFrameName, "Gems_%.2d.png", gemIndex);
//     auto sprite = Sprite::createWithSpriteFrameName(spriteFrameName);

    Cell& cell = _grid[CellY][CellX];
    cell._type = gemIndex;
    cell._count = 0;
    cell._sprite = sprite;
    if (nullptr != sprite)
    {
        // add it to scene & set position
        _board->addChild(sprite, 2);
        return true;
    }
    return false;
}

Vec2 Gems::getGemPosition(int CellX, int CellY)
{
    auto boardSize = _board->getContentSize();
    return Vec2((CellX + 0.5f) * boardSize.width / GRID_SIZE,
                (CellY + 0.5f) * boardSize.height / GRID_SIZE);
}

void Gems::onUpdateScores(float delta)
{
    if (_screenScore < _score)
    {
        _screenScore += Max(1, Min(int(250 * delta), _score - _screenScore));
        char screenScoreStr[64];
        sprintf(screenScoreStr, "Score: %d", _screenScore);
//         std::ostringstream screenScoreStream;
//         screenScoreStream << std::fixed << "Score: " << _screenScore;
//         auto screenScoreStr = screenScoreStream.str();
        _labelScore->setString(screenScoreStr);
        this->scheduleOnce(schedule_selector(Gems::onUpdateScores), UPDATE_SCORES_DELTA);
    }
}

int Gems::markSameColorGems(int CellX, int CellY)
{
    auto type = _grid[CellY][CellX]._type;
    int bound1, bound2;
    int count = markSameColorGemsHorz(CellX, CellY, type, false, bound1, bound2);
    if (0 == count)
    {
        count = markSameColorGemsVert(CellX, CellY, type, false, bound1, bound2);
    }
    return count;
}

int Gems::markSameColorGemsHorz(int CellX, int CellY, unsigned Type, bool HorzOnly, int& Left, int& Right)
{
    if (_grid[CellY][CellX]._type == Type
        && 0 == _grid[CellY][CellX]._count)
    {
        int left = CellX;
        if (CellX > 0)
        {
            for (int i = CellX-1; i >= Max(CellX-4, 0) && _grid[CellY][i]._type == Type; i--)
            {
                left = i;
            }
        }
        int right = CellX;
        for (int i = CellX; i < Min(CellX+5, GRID_SIZE) && _grid[CellY][i]._type == Type; i++)
        {
            right = i;
        }
        if (right - left + 1 >= 3)
        {
            int count = 0;
            int bottom[GRID_SIZE], top[GRID_SIZE];
            for (int i = left; i <= right; i++)
            {
                bottom[i] = top[i] = CellY;
                int vcount = 0;
                if (false == HorzOnly)
                {
                    vcount = markSameColorGemsVert(i, CellY, Type, true, bottom[i], top[i]);
                    count += vcount;
                }
                if (0 == vcount)
                {
                    count ++;
                }
            }
            if (false == HorzOnly)
            {
                for (int i = left; i <= right; i++)
                {
                    for (int j = bottom[i]; j <= top[i]; j++)
                    {
                        _grid[j][i]._count = count;
                    }
                }
            }

            Left = left;
            Right = right;
            return count;
        }
    }
    return 0;
}

int Gems::markSameColorGemsVert(int CellX, int CellY, unsigned Type, bool VertOnly, int& Bottom, int& Top)
{
    if (_grid[CellY][CellX]._type == Type
        && 0 == _grid[CellY][CellX]._count)
    {
        int bottom = CellY;
        if (CellY > 0)
        {
            for (int i = CellY-1; i >= Max(CellY-4, 0) && _grid[i][CellX]._type == Type; i--)
            {
                bottom = i;
            }
        }
        int top = CellY;
        for (int i = CellY; i < Min(CellY+5, GRID_SIZE) && _grid[i][CellX]._type == Type; i++)
        {
            top = i;
        }
        if (top - bottom + 1 >= 3)
        {
            int count = 0;
            int left[GRID_SIZE], right[GRID_SIZE];
            for (int i = bottom; i <= top; i++)
            {
                left[i] = right[i] = CellX;
                int hcount = 0;
                if (false == VertOnly)
                {
                    hcount = markSameColorGemsHorz(CellX, i, Type, true, left[i], right[i]);
                    count += hcount;
                }
                if (0 == hcount)
                {
                    count ++;
                }
            }
            if (false == VertOnly)
            {
                for (int j = bottom; j <= top; j++)
                {
                    for (int i = left[j]; i <= right[j]; i++)
                    {
                        _grid[j][i]._count = count;
                    }
                }
            }

            Bottom = bottom;
            Top = top;
            return count;
        }
    }
    return 0;
}

void Gems::removeMarked(bool UseAnimations)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    // remove marked gems
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            Cell& cell = _grid[j][i];
            if (cell._count > 0
                && nullptr != cell._sprite)
            {
                if (UseAnimations)
                {
                    auto index = cell._count;
                    char scoreStr[32] = "";
//                     std::ostringstream scoreStream;
                    if (index < ARRAYSIZE(GEM_SCORE))
                    {
                        int sc = GEM_SCORE[index];
                        _score += sc;
                        sprintf(scoreStr, "%d", sc);
//                         scoreStream << std::fixed << sc;
                    }
                    auto label = Label::createWithTTF(scoreStr, "fonts/Marker Felt.ttf", SCORE_LABELS_SIZE);
                    if (nullptr != label)
                    {
                        label->setPosition(cell._sprite->getPosition());
                        _board->addChild(label, 3);
                        Vec2 endPoint = label->getPosition();
                        float labelY = endPoint.y;
                        endPoint.y = origin.y + visibleSize.height + label->getContentSize().height;
                        float time = (endPoint.y - labelY) / LABEL_SPEED;
                        cocos2d::Vector<cocos2d::FiniteTimeAction*> labelActions;
                        labelActions.pushBack(cocos2d::MoveTo::create(time, endPoint));
                        labelActions.pushBack(cocos2d::RemoveSelf::create(true));
                        auto labelSequence = cocos2d::Sequence::create(labelActions);
                        label->runAction(labelSequence);
                    }

                    cocos2d::Vector<cocos2d::FiniteTimeAction*> gemActions;
                    gemActions.pushBack(cocos2d::ScaleTo::create(ANIMATION_DURATION, 0));
                    gemActions.pushBack(cocos2d::RemoveSelf::create(true));
                    auto gemSequence = cocos2d::Sequence::create(gemActions);
                    cell._sprite->stopAllActions();
                    cell._sprite->runAction(gemSequence);
                }
                else
                {
                    _board->removeChild(cell._sprite, true);
                }
                cell._sprite = nullptr;
            }
        }
    }
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (_grid[j][i]._count > 0)
            {
                // find filled cells above current
                int filledCell = -1;
                for (unsigned j2 = j+1; j2 < GRID_SIZE; j2++)
                {
                    if (0 == _grid[j2][i]._count)
                    {
                        filledCell = j2;
                        break;
                    }
                }
                unsigned j1 = j;
                if (filledCell >= 0)    // if some cells above are filled
                {
                    // copy existing gems into holes
                    for (unsigned j2 = filledCell; j2 < GRID_SIZE; j1++, j2++)
                    {
                        Cell& cell = _grid[j1][i];

                        _grid[j1][i] = _grid[j2][i];

                        if (nullptr != cell._sprite)
                        {
                            if (UseAnimations)
                            {
                                cell._sprite->stopAllActions();
                                auto action = cocos2d::MoveTo::create(ANIMATION_DURATION, getGemPosition(i, j1));
                                cell._sprite->runAction(action);
                            }
                            else
                            {
                                cell._sprite->setPosition(getGemPosition(i, j1));
                            }
                        }
                    }
                }
                for (; j1 < GRID_SIZE; j1++)
                {
                    fillCell(i, j1);
                    Cell& cell = _grid[j1][i];
                    if (nullptr != cell._sprite)
                    {
                        if (UseAnimations)
                        {
                            auto gemPos = getGemPosition(i, j1);
                            auto spriteSize = cell._sprite->getContentSize();
                            gemPos.y = origin.y + visibleSize.height + spriteSize.height;
                            cell._sprite->setPosition(gemPos);
                            auto action = cocos2d::MoveTo::create(ANIMATION_DURATION, getGemPosition(i, j1));
                            cell._sprite->runAction(action);
                        }
                        else
                        {
                            cell._sprite->setPosition(getGemPosition(i, j1));
                        }
                    }
                }
            }
        }
    }
    this->scheduleOnce(schedule_selector(Gems::onUpdateScores), UPDATE_SCORES_DELTA);
}

bool Gems::onTouchBegan(Touch* touch, Event* event)
{
    if (false == _ignoreInput)
    {
        auto bounds = event->getCurrentTarget()->getBoundingBox();

        if (bounds.containsPoint(touch->getLocation()))
        {
            // touch inside board
            auto target = event->getCurrentTarget();
            auto localPos = target->convertToNodeSpace(touch->getLocation());
            auto boardSize = _board->getContentSize();
            // get touch cell coordinates
            _cellToX = int(localPos.x * GRID_SIZE / boardSize.width);
            _cellToY = int(localPos.y * GRID_SIZE / boardSize.height);
            Cell& cellFrom = _grid[_cellFromY][_cellFromX];
            Cell& cellTo = _grid[_cellToY][_cellToX];
            // if gem selected
            if (_cellFromX >= 0
                && _cellFromY >= 0
                && nullptr != cellFrom._sprite)
            {   // remove scaling
                cellFrom._sprite->stopAllActions();
                auto action = cocos2d::ScaleTo::create(0.5f*ANIMATION_DURATION, 1);
                cellFrom._sprite->runAction(action);
            }
            // if selected cell clicked again
            if (_cellFromX >= 0
                && _cellFromY >= 0
                && _cellToX == _cellFromX
                && _cellToY == _cellFromY
                && nullptr != cellFrom._sprite)
            {   // remove selection, and set scale to 1
                auto action = cocos2d::ScaleTo::create(0.5f*ANIMATION_DURATION, 1);
                cellFrom._sprite->runAction(action);

                _cellFromX = _cellFromY = -1;
            }
            else    // clicked not selected cell
            {   // test if it's neighbour of selected
                if (_cellFromX >= 0
                    && _cellFromY >= 0
                    && nullptr != cellFrom._sprite
                    && ((_cellToX == _cellFromX && fabs(_cellToY - _cellFromY) == 1)
                        || (_cellToY == _cellFromY && fabs(_cellToX - _cellFromX) == 1)))
                {   // it is, disable input while move is animated
                    _ignoreInput = true;
                    // start gems exchange animation
                    cocos2d::Vector<cocos2d::FiniteTimeAction*> actions1, actions2;
                    actions1.pushBack(cocos2d::MoveTo::create(ANIMATION_DURATION, getGemPosition(_cellToX, _cellToY)));
                    actions1.pushBack(cocos2d::ScaleTo::create(0.5f * ANIMATION_DURATION, 1));
                    auto sequence1 = cocos2d::Sequence::create(actions1);
                    cellFrom._sprite->stopAllActions();
                    cellFrom._sprite->runAction(sequence1);

                    actions2.pushBack(cocos2d::MoveTo::create(ANIMATION_DURATION, getGemPosition(_cellFromX, _cellFromY)));
                    actions2.pushBack(cocos2d::ScaleTo::create(0.5f * ANIMATION_DURATION, 1));
                    auto sequence2 = cocos2d::Sequence::create(actions2);
                    cellTo._sprite->stopAllActions();
                    cellTo._sprite->runAction(sequence2);
                    // schedule move processing when animation is complete
                    this->scheduleOnce(schedule_selector(Gems::onExchangeComplete), 1.5f*ANIMATION_DURATION);
                }
                else    // clicked gem is not neighbour of selected, select it
                {
                    // remove old selection
                    if (_cellFromX >= 0
                        && _cellFromY >= 0
                        && nullptr != cellFrom._sprite)
                    {
                        auto action = cocos2d::ScaleTo::create(0.5f*ANIMATION_DURATION, 1);
                        cellFrom._sprite->runAction(action);
                    }
                    // remember new
                    _cellFromX = _cellToX;
                    _cellFromY = _cellToY;
                    Cell& cellFrom = _grid[_cellFromY][_cellFromX];
                    if (nullptr != cellFrom._sprite)
                    {
                        // animate scaling
                        cellFrom._sprite->stopAllActions();
                        auto action = cocos2d::ScaleTo::create(ANIMATION_DURATION, SELECTED_SCALE);
                        cellFrom._sprite->runAction(action);
                    }
                }
            }
        }
    }
    return true;
}

void Gems::clearGridCounts()
{
    for (int j = 0; j < GRID_SIZE; j++)
    {
        for (int i = 0; i < GRID_SIZE; i++)
        {
            _grid[j][i]._count = 0;
        }
    }
}

void Gems::processMove(bool UseAnimations)
{
    // exchange gems
    Cell cell = _grid[_cellToY][_cellToX];
    _grid[_cellToY][_cellToX] = _grid[_cellFromY][_cellFromX];
    _grid[_cellFromY][_cellFromX] = cell;

    int count1 = markSameColorGems(_cellToX, _cellToY);
    int count2 = markSameColorGems(_cellFromX, _cellFromY);
    if (0 == count1
        && 0 == count2)
    {
        Cell cell = _grid[_cellToY][_cellToX];
        _grid[_cellToY][_cellToX] = _grid[_cellFromY][_cellFromX];
        _grid[_cellFromY][_cellFromX] = cell;
        Cell& cellFrom = _grid[_cellFromY][_cellFromX];
        Cell& cellTo = _grid[_cellToY][_cellToX];
        if (nullptr != cellFrom._sprite
            && nullptr != cellTo._sprite)
        {
            if (false != UseAnimations)
            {
                cellTo._sprite->stopAllActions();
                cocos2d::Vector<cocos2d::FiniteTimeAction*> actions1;
                actions1.pushBack(cocos2d::MoveTo::create(ANIMATION_DURATION, getGemPosition(_cellToX, _cellToY)));
                actions1.pushBack(cocos2d::ScaleTo::create(0.5f*ANIMATION_DURATION, 1));
                auto sequence1 = cocos2d::Sequence::create(actions1);
                cellTo._sprite->runAction(sequence1);

                cellFrom._sprite->stopAllActions();
                cocos2d::Vector<cocos2d::FiniteTimeAction*> actions2;
                actions2.pushBack(cocos2d::MoveTo::create(ANIMATION_DURATION, getGemPosition(_cellFromX, _cellFromY)));
                actions2.pushBack(cocos2d::ScaleTo::create(0.5f*ANIMATION_DURATION, 1));
                auto sequence2 = cocos2d::Sequence::create(actions2);
                cellFrom._sprite->runAction(sequence2);
            }
            else
            {
                cellTo._sprite->setPosition(getGemPosition(_cellToX, _cellToY));
                cellFrom._sprite->setPosition(getGemPosition(_cellFromX, _cellFromY));
            }
        }
    }
    else
    {
        removeMarked(true);
    }

    _cellFromX = _cellFromY = -1;
    // schedule another try to remove gems after gems fall animation
    this->scheduleOnce(schedule_selector(Gems::onMoveComplete), 1.5f*ANIMATION_DURATION);
}

void Gems::onExchangeComplete(float /*delta*/)
{
    processMove(true);
}

void Gems::onMoveComplete(float delta)
{
    // test if there are gems to remove after filling the gaps
    clearGridCounts();
    int count = 0;
    for (int j = 0; j < GRID_SIZE; j++)
    {
        for (int i = 0; i < GRID_SIZE; i++)
        {
            if (0 == _grid[j][i]._count)
            {
                count += markSameColorGems(i, j);
            }
        }
    }
    removeMarked(true);
    // if there were gems to remove schedule another check
    if (count > 0)
    {
        this->scheduleOnce(schedule_selector(Gems::onMoveComplete), 1.5f*ANIMATION_DURATION);
    }
    else
    {   // enable input
        _ignoreInput = false;
    }
}

void Gems::menuCloseCallback(Ref* pSender)
{
    // quit the application
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    //To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}
