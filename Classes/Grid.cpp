/****************************************************************************
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "Grid.h"

USING_NS_CC;

// for 0, 1, 2, 3, 4, 5, 6, 7 gems in row
const unsigned GEM_SCORE[] = { 0, 0, 0, 15, 20, 30, 50, 100, 250, 500, 1000 };

const int LABEL_SPEED = 200;
const int SCORE_LABELS_SIZE = 16;
const float SELECTED_SCALE = 1.1f;

static int Min(int V1, int V2) { return (V1 < V2) ? V1 : V2; }
static int Max(int V1, int V2) { return (V1 > V2) ? V1 : V2; }

Grid::Grid()
{
    _ignoreInput = false;
    _scoreInc = 0;
}

bool Grid::SetupField(cocos2d::Sprite* Board)
{
    _board = Board;
    _gems = Director::getInstance()->getTextureCache()->addImage("Gems.png");

    if (nullptr != _gems)
    {
//         auto spritecache = SpriteFrameCache::getInstance();
//         spritecache->addSpriteFramesWithFile("Gems.plist");
//         
//         for (int i = 0; ; i++)
//         {
//             char spriteFrameName[32];
//             sprintf(spriteFrameName, "Gems_%.2d.png", i);
//             SpriteFrame* sf = spritecache->getSpriteFrameByName(spriteFrameName);
//             if (nullptr == sf)
//             {
//                 _gemsCount = i;
//                 break;
//             }
//         }

        // generate gems grid
        for (unsigned j = 0; j < ARRAYSIZE(_grid); j++)
        {
            for (unsigned i = 0; i < ARRAYSIZE(_grid[j]); i++)
            {
                fillCell(i, j);
                if (nullptr != _grid[j][i].GetSprite())
                {
                    _grid[j][i].GetSprite()->setPosition(GetGemPosition(i, j));
                }
            }
        }
        // make field stable
        while (MarkSameColorGems() > 0)
        {
            RemoveMarked(false);
        }
        return true;
    }
    
    return false;
}


bool Grid::fillCell(int CellX, int CellY)
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
    cell.SetType(gemIndex);
    cell.SetCount(0);
    cell.SetSprite(sprite);
    if (nullptr != sprite)
    {
        // add it to scene & set position
        _board->addChild(sprite, 2);
        return true;
    }
    return false;
}

Vec2 Grid::GetGemPosition(int CellX, int CellY) const
{
    auto boardSize = _board->getContentSize();
    return Vec2((CellX + 0.5f) * boardSize.width / GRID_SIZE,
                (CellY + 0.5f) * boardSize.height / GRID_SIZE);
}

void Grid::ClearGridCounts()
{
    for (int j = 0; j < GRID_SIZE; j++)
    {
        for (int i = 0; i < GRID_SIZE; i++)
        {
            _grid[j][i].SetCount(0);
        }
    }
}

int Grid::MarkSameColorGems()
{
    int count = 0;
    for (int j = 0; j < GRID_SIZE; j++)
    {
        for (int i = 0; i < GRID_SIZE; i++)
        {
            if (0 == _grid[j][i].GetCount())
            {
                count += markSameColorGems(i, j);
            }
        }
    }
    return count;
}

int Grid::markSameColorGems(int CellX, int CellY)
{
    auto type = _grid[CellY][CellX].GetType();
    int bound1, bound2;
    int count = markSameColorGemsHorz(CellX, CellY, type, false, bound1, bound2);
    if (0 == count)
    {
        count = markSameColorGemsVert(CellX, CellY, type, false, bound1, bound2);
    }
    return count;
}

int Grid::markSameColorGemsHorz(int CellX, int CellY, unsigned Type, bool HorzOnly, int& Left, int& Right)
{
    if (_grid[CellY][CellX].GetType() == Type
        && 0 == _grid[CellY][CellX].GetCount())
    {
        int left = CellX;
        if (CellX > 0)
        {
            for (int i = CellX-1; i >= Max(CellX-4, 0) && _grid[CellY][i].GetType() == Type; i--)
            {
                left = i;
            }
        }
        int right = CellX;
        for (int i = CellX; i < Min(CellX+5, GRID_SIZE) && _grid[CellY][i].GetType() == Type; i++)
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
                        _grid[j][i].SetCount(count);
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

int Grid::markSameColorGemsVert(int CellX, int CellY, unsigned Type, bool VertOnly, int& Bottom, int& Top)
{
    if (_grid[CellY][CellX].GetType() == Type
        && 0 == _grid[CellY][CellX].GetCount())
    {
        int bottom = CellY;
        if (CellY > 0)
        {
            for (int i = CellY-1; i >= Max(CellY-4, 0) && _grid[i][CellX].GetType() == Type; i--)
            {
                bottom = i;
            }
        }
        int top = CellY;
        for (int i = CellY; i < Min(CellY+5, GRID_SIZE) && _grid[i][CellX].GetType() == Type; i++)
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
                        _grid[j][i].SetCount(count);
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

void Grid::RemoveMarked(bool UseAnimations)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    // remove marked gems
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            Cell& cell = _grid[j][i];
            if (cell.GetCount() > 0
                && nullptr != cell.GetSprite())
            {
                if (UseAnimations)
                {
                    auto index = cell.GetCount();
                    char scoreStr[32] = "";
//                     std::ostringstream scoreStream;
                    if (index < ARRAYSIZE(GEM_SCORE))
                    {
                        int sc = GEM_SCORE[index];
                        _scoreInc += sc;
                        sprintf(scoreStr, "%d", sc);
//                         scoreStream << std::fixed << sc;
                    }
                    auto label = Label::createWithTTF(scoreStr, "fonts/Marker Felt.ttf", SCORE_LABELS_SIZE);
                    if (nullptr != label)
                    {
                        label->setPosition(cell.GetSprite()->getPosition());
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
                    cell.GetSprite()->stopAllActions();
                    cell.GetSprite()->runAction(gemSequence);
                }
                else
                {
                    _board->removeChild(cell.GetSprite(), true);
                }
                cell.SetSprite(nullptr);
            }
        }
    }
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (_grid[j][i].GetCount() > 0)
            {
                // find filled cells above current
                int filledCell = -1;
                for (unsigned j2 = j+1; j2 < GRID_SIZE; j2++)
                {
                    if (0 == _grid[j2][i].GetCount())
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

                        if (nullptr != cell.GetSprite())
                        {
                            if (UseAnimations)
                            {
                                cell.GetSprite()->stopAllActions();
                                auto action = cocos2d::MoveTo::create(ANIMATION_DURATION, GetGemPosition(i, j1));
                                cell.GetSprite()->runAction(action);
                            }
                            else
                            {
                                cell.GetSprite()->setPosition(GetGemPosition(i, j1));
                            }
                        }
                    }
                }
                for (; j1 < GRID_SIZE; j1++)
                {
                    fillCell(i, j1);
                    Cell& cell = _grid[j1][i];
                    if (nullptr != cell.GetSprite())
                    {
                        if (UseAnimations)
                        {
                            auto gemPos = GetGemPosition(i, j1);
                            auto spriteSize = cell.GetSprite()->getContentSize();
                            gemPos.y = origin.y + visibleSize.height + spriteSize.height;
                            cell.GetSprite()->setPosition(gemPos);
                            auto action = cocos2d::MoveTo::create(ANIMATION_DURATION, GetGemPosition(i, j1));
                            cell.GetSprite()->runAction(action);
                        }
                        else
                        {
                            cell.GetSprite()->setPosition(GetGemPosition(i, j1));
                        }
                    }
                }
            }
        }
    }
}

bool Grid::onTouchBegan(Touch* touch, cocos2d::Event* event, bool &scheduleProcessMove)
{
    scheduleProcessMove = false;
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
            _selTo.Set(int(localPos.x * GRID_SIZE / boardSize.width),
                       int(localPos.y * GRID_SIZE / boardSize.height));
            Cell& cellFrom = _grid[_selFrom.GetY()][_selFrom.GetX()];
            Cell& cellTo = _grid[_selTo.GetY()][_selTo.GetX()];
            // if gem selected
            if (_selFrom.IsSet()
                && nullptr != cellFrom.GetSprite())
            {   // remove scaling
                cellFrom.GetSprite()->stopAllActions();
                auto action = cocos2d::ScaleTo::create(0.5f*ANIMATION_DURATION, 1);
                cellFrom.GetSprite()->runAction(action);
            }
            // if selected cell clicked again
            if (_selFrom.IsSet()
                && _selTo == _selFrom
                && nullptr != cellFrom.GetSprite())
            {   // remove selection, and set scale to 1
                auto action = cocos2d::ScaleTo::create(0.5f*ANIMATION_DURATION, 1);
                cellFrom.GetSprite()->runAction(action);

                _selFrom.Reset();
            }
            else    // clicked not selected cell
            {   // test if it's neighbour of selected
                if (_selFrom.IsSet()
                    && nullptr != cellFrom.GetSprite()
                    && _selTo.IsNextBy(_selFrom))
                {   // it is, disable input while move is animated
                    _ignoreInput = true;
                    // start gems exchange animation
                    cocos2d::Vector<cocos2d::FiniteTimeAction*> actions1, actions2;
                    actions1.pushBack(cocos2d::MoveTo::create(ANIMATION_DURATION, GetGemPosition(_selTo.GetX(), _selTo.GetY())));
                    actions1.pushBack(cocos2d::ScaleTo::create(0.5f * ANIMATION_DURATION, 1));
                    auto sequence1 = cocos2d::Sequence::create(actions1);
                    cellFrom.GetSprite()->stopAllActions();
                    cellFrom.GetSprite()->runAction(sequence1);

                    actions2.pushBack(cocos2d::MoveTo::create(ANIMATION_DURATION, GetGemPosition(_selFrom.GetX(), _selFrom.GetY())));
                    actions2.pushBack(cocos2d::ScaleTo::create(0.5f * ANIMATION_DURATION, 1));
                    auto sequence2 = cocos2d::Sequence::create(actions2);
                    cellTo.GetSprite()->stopAllActions();
                    cellTo.GetSprite()->runAction(sequence2);
                    // schedule move processing when animation is complete
                    scheduleProcessMove = true;
                }
                else    // clicked gem is not neighbour of selected, select it
                {
                    // remove old selection
                    if (_selFrom.IsSet()
                        && nullptr != cellFrom.GetSprite())
                    {
                        auto action = cocos2d::ScaleTo::create(0.5f*ANIMATION_DURATION, 1);
                        cellFrom.GetSprite()->runAction(action);
                    }
                    // remember new
                    _selFrom = _selTo;
                    Cell& cellFrom = _grid[_selFrom.GetY()][_selFrom.GetX()];
                    if (nullptr != cellFrom.GetSprite())
                    {
                        // animate scaling
                        cellFrom.GetSprite()->stopAllActions();
                        auto action = cocos2d::ScaleTo::create(ANIMATION_DURATION, SELECTED_SCALE);
                        cellFrom.GetSprite()->runAction(action);
                    }
                }
            }
        }
    }
    return true;
}

void Grid::ProcessMove(bool UseAnimations, bool& ScheduleUpdateScore)
{
    ScheduleUpdateScore = false;
    // exchange gems
    Cell cell = _grid[_selTo.GetY()][_selTo.GetX()];
    _grid[_selTo.GetY()][_selTo.GetX()] = _grid[_selFrom.GetY()][_selFrom.GetX()];
    _grid[_selFrom.GetY()][_selFrom.GetX()] = cell;

    int count1 = markSameColorGems(_selTo.GetX(), _selTo.GetY());
    int count2 = markSameColorGems(_selFrom.GetX(), _selFrom.GetY());
    if (0 == count1
        && 0 == count2)
    {
        Cell cell = _grid[_selTo.GetY()][_selTo.GetX()];
        _grid[_selTo.GetY()][_selTo.GetX()] = _grid[_selFrom.GetY()][_selFrom.GetX()];
        _grid[_selFrom.GetY()][_selFrom.GetX()] = cell;
        Cell& cellFrom = _grid[_selFrom.GetY()][_selFrom.GetX()];
        Cell& cellTo = _grid[_selTo.GetY()][_selTo.GetX()];
        if (nullptr != cellFrom.GetSprite()
            && nullptr != cellTo.GetSprite())
        {
            if (false != UseAnimations)
            {
                cellTo.GetSprite()->stopAllActions();
                cocos2d::Vector<cocos2d::FiniteTimeAction*> actions1;
                actions1.pushBack(cocos2d::MoveTo::create(ANIMATION_DURATION, GetGemPosition(_selTo.GetX(), _selTo.GetY())));
                actions1.pushBack(cocos2d::ScaleTo::create(0.5f*ANIMATION_DURATION, 1));
                auto sequence1 = cocos2d::Sequence::create(actions1);
                cellTo.GetSprite()->runAction(sequence1);

                cellFrom.GetSprite()->stopAllActions();
                cocos2d::Vector<cocos2d::FiniteTimeAction*> actions2;
                actions2.pushBack(cocos2d::MoveTo::create(ANIMATION_DURATION, GetGemPosition(_selFrom.GetX(), _selFrom.GetY())));
                actions2.pushBack(cocos2d::ScaleTo::create(0.5f*ANIMATION_DURATION, 1));
                auto sequence2 = cocos2d::Sequence::create(actions2);
                cellFrom.GetSprite()->runAction(sequence2);
            }
            else
            {
                cellTo.GetSprite()->setPosition(GetGemPosition(_selTo.GetX(), _selTo.GetY()));
                cellFrom.GetSprite()->setPosition(GetGemPosition(_selFrom.GetX(), _selFrom.GetY()));
            }
        }
    }
    else
    {
        RemoveMarked(true);
        ScheduleUpdateScore = true;
    }

    _selFrom.Reset();
}
