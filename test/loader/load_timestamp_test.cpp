#include "gtest/gtest.h"
#include "test/test_utility/include/test_helper.h"

#include "src/common/include/date.h"
#include "src/common/include/time.h"
#include "src/common/include/timestamp.h"

using namespace std;
using namespace graphflow::common;
using namespace graphflow::storage;
using namespace graphflow::testing;

class TinySnbTimestampTest : public InMemoryDBLoadedTest {

public:
    string getInputCSVDir() override { return "dataset/tinysnb/"; }
};

// Warning: This test assumes that each line in tinysnb's vPerson.csv gets
// the node offsets that start from 0 consecutively (so first line gets person ID 0, second person
// ID 1, so on and so forth).
TEST_F(TinySnbTimestampTest, NodePropertyColumnWithTimestamp) {
    auto& catalog = defaultSystem->graph->getCatalog();
    auto label = catalog.getNodeLabelFromString("person");
    auto propertyIdx = catalog.getNodeProperty(label, "registerTime");
    auto col = defaultSystem->graph->getNodesStore().getNodePropertyColumn(label, propertyIdx.id);
    NumericMetric numBufferHits(true), numBufferMisses(true), numIO(true);
    BufferManagerMetrics metrics(numBufferHits, numBufferMisses, numIO);
    EXPECT_EQ(Timestamp::FromDatetime(Date::FromDate(2011, 8, 20), Time::FromTime(11, 25, 30)),
        col->readValue(0, metrics).val.timestampVal);
    EXPECT_EQ(
        Timestamp::FromDatetime(Date::FromDate(2008, 11, 3), Time::FromTime(13, 25, 30, 526)).value,
        col->readValue(1, metrics).val.timestampVal.value);
    EXPECT_EQ(Timestamp::FromDatetime(Date::FromDate(1911, 8, 20), Time::FromTime(2, 32, 21)),
        col->readValue(2, metrics).val.timestampVal);
    EXPECT_EQ(Timestamp::FromDatetime(Date::FromDate(2031, 11, 30), Time::FromTime(12, 25, 30)),
        col->readValue(3, metrics).val.timestampVal);
    EXPECT_EQ(Timestamp::FromDatetime(Date::FromDate(1976, 12, 23), Time::FromTime(11, 21, 42)),
        col->readValue(4, metrics).val.timestampVal);
    EXPECT_EQ(
        Timestamp::FromDatetime(Date::FromDate(1972, 7, 31), Time::FromTime(13, 22, 30, 678559)),
        col->readValue(5, metrics).val.timestampVal);
    EXPECT_EQ(Timestamp::FromDatetime(Date::FromDate(1976, 12, 23), Time::FromTime(11, 21, 42)),
        col->readValue(6, metrics).val.timestampVal);
    EXPECT_EQ(Timestamp::FromDatetime(Date::FromDate(2023, 2, 21), Time::FromTime(13, 25, 30)),
        col->readValue(7, metrics).val.timestampVal);
}