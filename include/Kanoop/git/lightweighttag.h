/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a concrete implemetation of a lightweight tag.
 * As such, it is a trivial subclass of the abstract Tag class.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Stephen Punak, August 1, 2024
*/

#ifndef LIGHTWEIGHTTAG_H
#define LIGHTWEIGHTTAG_H
#include <Kanoop/git/tag.h>
#include <Kanoop/git/gittypes.h>

namespace GIT {

class Repository;
class LightweightTag : public Tag
{
public:
    LightweightTag();
    LightweightTag(Repository* repo, const QString& name, const ObjectId& objectId);
    LightweightTag(const LightweightTag& other);
    LightweightTag& operator=(const LightweightTag& other);
    virtual ~LightweightTag();

private:
    void commonInit();
};

} // namespace GIT

#endif // LIGHTWEIGHTTAG_H
