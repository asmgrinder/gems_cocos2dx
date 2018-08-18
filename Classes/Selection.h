/****************************************************************************
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#ifndef __SELECTION_H__
#define __SELECTION_H__

class Selection
{
public:
    Selection();
    Selection(int x, int y);
    void Set(int x, int y);
    bool IsNextBy(Selection& other) const;
    void Reset();
    bool IsSet() const;
    int GetX() const { return _x; }
    int GetY() const { return _y; }
    bool operator == (const Selection &sel) { return _x == sel._x && _y == sel._y; }
protected:
    int _x, _y;
};

#endif
