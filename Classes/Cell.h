/****************************************************************************
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#ifndef __CELL_H__
#define __CELL_H__

#include "cocos2d.h"

class Cell
{
public:
    Cell();
    cocos2d::Sprite* GetSprite();
    unsigned int GetType();
    unsigned GetCount();
    void SetSprite(cocos2d::Sprite* Sprite);
    void SetType(unsigned int Type);
    void SetCount(unsigned Count);
    void Swap(Cell& cell);
protected:
    cocos2d::Sprite* _sprite;
    unsigned int _type;
    unsigned _count;
};
#endif
