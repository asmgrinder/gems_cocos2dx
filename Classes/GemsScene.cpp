/****************************************************************************
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

const float UPDATE_SCORES_DELTA = 0.05f;

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
    _score = _screenScore = 0;
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
        // position the label on the top center of the screen
        _labelScore->setPosition(Vec2(origin.x + 0.5f * visibleSize.width,
                                origin.y + 0.5f * (visibleSize.height + boardBorders->getContentSize().height + _labelScore->getContentSize().height)));

        // add the label as a child to this layer
        this->addChild(_labelScore, 2);
    }

    _grid.SetupField(_board);
    return true;
}

bool Gems::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    bool scheduleProcessMove = false;
    _grid.onTouchBegan(touch, event, scheduleProcessMove);
    if (false != scheduleProcessMove)
    {
        // schedule move processing when animation is complete
        this->scheduleOnce(schedule_selector(Gems::onExchangeComplete), 1.5f*ANIMATION_DURATION);
    }

    return true;
}

void Gems::onExchangeComplete(float /*delta*/)
{
    bool scheduleUpdateScore = false;
    _grid.ProcessMove(true, scheduleUpdateScore);
    if (false != scheduleUpdateScore)
    {
        _score += _grid.GetScoreInc();
        _grid.SetScoreInc(0);
        this->scheduleOnce(schedule_selector(Gems::onUpdateScores), UPDATE_SCORES_DELTA);
    }

    // schedule another try to remove gems after gems fall animation
    this->scheduleOnce(schedule_selector(Gems::onMoveComplete), 1.5f * ANIMATION_DURATION);
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

void Gems::onMoveComplete(float delta)
{
    // test if there are gems to remove after filling the gaps
    _grid.ClearGridCounts();
    int count =_grid.MarkSameColorGems();
    _grid.RemoveMarked(true);
    _score += _grid.GetScoreInc();
    _grid.SetScoreInc(0);
    this->scheduleOnce(schedule_selector(Gems::onUpdateScores), UPDATE_SCORES_DELTA);
    // if there were gems to remove schedule another check
    if (count > 0)
    {
        this->scheduleOnce(schedule_selector(Gems::onMoveComplete), 1.5f*ANIMATION_DURATION);
    }
    else
    {   // enable input
        _grid.SetIgnoreInput(false);
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
