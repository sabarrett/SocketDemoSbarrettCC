#include "Trackable.h"
#include <unordered_map>

class GraphicsBuffer;

class GraphicsBufferManager : public Trackable
{

public:
    GraphicsBufferManager();
    ~GraphicsBufferManager();

    int getNumOfGraphicsBuffers() { return mGraphicsBuffers.size(); }

    GraphicsBuffer* createGraphicsBuffer(std::string filename);
    GraphicsBuffer* createAndManageGraphicsBuffer(std::string key, std::string filename);

    void addGraphicsBuffer(std::string key, GraphicsBuffer* gb);
    void removeGraphicsBuffer(std::string key);
    void removeAndDeleteGraphicsBuffer(std::string key);
    void deleteGraphicsBuffer(GraphicsBuffer* gb);

    std::string findGraphicsBufferKey(GraphicsBuffer* gb);
    GraphicsBuffer* getGraphicsBuffer(std::string key);

    void clear();

private:
    std::unordered_map<std::string, GraphicsBuffer*> mGraphicsBuffers;

};