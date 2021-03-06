/****************************************************************************
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
#include "Grid.h"

class Gems : public cocos2d::Scene
{
public:

    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(Gems);

protected:
    virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
    void onExchangeComplete(float delta);
    void onMoveComplete(float delta);
    void onUpdateScores(float delta);

    cocos2d::Sprite* _board;
    Grid _grid;
    int _score, _screenScore;
    cocos2d::Label* _labelScore;
};

#endif // __GEMS_SCENE_H__
