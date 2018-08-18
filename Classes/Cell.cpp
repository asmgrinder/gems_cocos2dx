/****************************************************************************
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "Cell.h"


Cell::Cell()
{
}

cocos2d::Sprite* Cell::GetSprite()
{
    return _sprite;
}

unsigned int Cell::GetType()
{
    return _type;
}

unsigned Cell::GetCount()
{
    return _count;
}

void Cell::SetSprite(cocos2d::Sprite* Sprite)
{
    _sprite = Sprite;
}

void Cell::SetType(unsigned int Type)
{
    _type = Type;
}

void Cell::SetCount(unsigned Count)
{
    _count = Count;
}

void Cell::Swap(Cell& other)
{
    Cell tmp = other;
    *this = other;
    other = tmp;
}
