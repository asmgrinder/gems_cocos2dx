/****************************************************************************
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#ifndef __GRID_H__
#define __GRID_H__

#include "cocos2d.h"
#include "Cell.h"
#include "Selection.h"

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

const int GRID_SIZE = 9;
const float ANIMATION_DURATION = 0.5f;

class Grid
{
public:
    Grid();
    bool SetupField(cocos2d::Sprite* Board);
    cocos2d::Vec2 GetGemPosition(int CellX, int CellY) const;
    cocos2d::Vec2 GetGemPosition(Selection &sel) const { return GetGemPosition(sel.GetX(), sel.GetY()); }
    void ClearGridCounts();
    int MarkSameColorGems();
    void RemoveMarked(bool UseAnimations);
    int GetScoreInc() const { return _scoreInc; }
    void SetScoreInc(int Value) { _scoreInc = Value; }
    void ProcessMove(bool UseAnimations, bool& ScheduleUpdateScore);
    void SetIgnoreInput(bool Ignore) { _ignoreInput = Ignore; }
    bool GetIgnoreInput() const { return _ignoreInput; }
    virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*, bool &scheduleProcessMove);

protected:
    int markSameColorGems(int CellX, int CellY);
    int markSameColorGemsHorz(int CellX, int CellY, unsigned Type, bool HorzOnly, int& Left, int& Right);
    int markSameColorGemsVert(int CellX, int CellY, unsigned Type, bool VertOnly, int& Bottom, int& Top);
    bool fillCell(int CellX, int CellY);
    //Cell& getCell(Selection sel) { return _grid[sel.GetY()][sel.GetX()]; }
    bool _ignoreInput;
    int _scoreInc;
    Selection _selFrom, _selTo;
    //int _cellFromX, _cellFromY, _cellToX, _cellToY;
    Cell _grid[GRID_SIZE][GRID_SIZE];
    cocos2d::Texture2D* _gems;
    cocos2d::Sprite* _board;
};

#endif
