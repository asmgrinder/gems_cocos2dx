/****************************************************************************
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "Selection.h" 

Selection::Selection()
{
    Reset();
}

Selection::Selection(int x, int y)
{
    Set(x, y);
}

void Selection::Set(int x, int y)
{
    _x = x;
    _y = y;
}

void Selection::Reset()
{
    _x = -1;
    _y = -1;
}

bool Selection::IsSet() const
{
    return _x >= 0 && _y >= 0;
}

bool Selection::IsNextBy(Selection& other) const
{
    if (IsSet() && other.IsSet()
        && ((_x == other._x && (_y + 1 == other._y  || other._y + 1 == _y))
            || (_y == other._y && (_x + 1 == other._x  || other._x + 1 == _x))))
    {
        return true;
    }
    return false;
}
