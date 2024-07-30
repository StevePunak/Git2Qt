#ifndef STAGEOPTIONS_H
#define STAGEOPTIONS_H

namespace GIT {

class StageOptions
{
public:
    StageOptions() :
        _includeIgnored(false) {}

    bool includeIgnored() const { return _includeIgnored; }
    void setIncludeIgnored(bool value) { _includeIgnored = value; }

private:
    bool _includeIgnored;
};

} // namespace GIT

#endif // STAGEOPTIONS_H
