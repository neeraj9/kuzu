#include "test/test_utility/include/test_helper.h"

using namespace graphflow::testing;

class TinySnbUpdateTest : public DBTest {
public:
    string getInputCSVDir() override { return "dataset/tinysnb/"; }

    string getStringExceedsOverflow() {
        string veryLongList = "[";
        for (int i = 0; i < 599; ++i) {
            veryLongList += to_string(i);
            veryLongList += ",";
        }
        veryLongList += "599]";
        return veryLongList;
    }
};

// SET clause tests

TEST_F(TinySnbUpdateTest, SetNodeIntPropTest) {
    conn->query("MATCH (a:person) WHERE a.ID=0 SET a.age=20 + 50");
    auto result = conn->query("MATCH (a:person) WHERE a.ID=0 RETURN a.age");
    ASSERT_EQ(result->getNext()->getResultValue(0)->getInt64Val(), 70);
}

TEST_F(TinySnbUpdateTest, SetNodeDoublePropTest) {
    conn->query("MATCH (a:person) WHERE a.ID=0 SET a.eyeSight=1.0");
    auto result = conn->query("MATCH (a:person) WHERE a.ID=0 RETURN a.eyeSight");
    ASSERT_EQ(result->getNext()->getResultValue(0)->getDoubleVal(), 1.0);
}

TEST_F(TinySnbUpdateTest, SetNodeBoolPropTest) {
    conn->query("MATCH (a:person) WHERE a.ID=0 SET a.isStudent=false");
    auto result = conn->query("MATCH (a:person) WHERE a.ID=0 RETURN a.isStudent");
    ASSERT_EQ(result->getNext()->getResultValue(0)->getBooleanVal(), false);
}

TEST_F(TinySnbUpdateTest, SetNodeDatePropTest) {
    conn->query("MATCH (a:person) WHERE a.ID=0 SET a.birthdate=date('2200-10-10')");
    auto result = conn->query("MATCH (a:person) WHERE a.ID=0 RETURN a.birthdate");
    ASSERT_EQ(result->getNext()->getResultValue(0)->getDateVal(), Date::FromDate(2200, 10, 10));
}

TEST_F(TinySnbUpdateTest, SetNodeTimestampPropTest) {
    conn->query(
        "MATCH (a:person) WHERE a.ID=0 SET a.registerTime=timestamp('2200-10-10 12:01:01')");
    auto result = conn->query("MATCH (a:person) WHERE a.ID=0 RETURN a.registerTime");
    ASSERT_EQ(result->getNext()->getResultValue(0)->getTimestampVal(),
        Timestamp::FromDatetime(Date::FromDate(2200, 10, 10), Time::FromTime(12, 1, 1)));
}

TEST_F(TinySnbUpdateTest, SetNodeShortStringPropTest) {
    conn->query("MATCH (a:person) WHERE a.ID=0 SET a.fName='abcdef'");
    auto result = conn->query("MATCH (a:person) WHERE a.ID=0 RETURN a.fName");
    ASSERT_EQ(result->getNext()->getResultValue(0)->getStringVal(), "abcdef");
}

TEST_F(TinySnbUpdateTest, SetNodeLongStringPropTest) {
    conn->query("MATCH (a:person) WHERE a.ID=0 SET a.fName='abcdefghijklmnopqrstuvwxyz'");
    auto result = conn->query("MATCH (a:person) WHERE a.ID=0 RETURN a.fName");
    ASSERT_EQ(result->getNext()->getResultValue(0)->getStringVal(), "abcdefghijklmnopqrstuvwxyz");
}

TEST_F(TinySnbUpdateTest, SetNodeListOfIntPropTest) {
    conn->query("MATCH (a:person) WHERE a.ID=0 SET a.workedHours=[10,20]");
    auto result = conn->query("MATCH (a:person) WHERE a.ID=0 RETURN a.workedHours");
    auto value = result->getNext()->getResultValue(0);
    ASSERT_EQ(value->to_string(), "[10,20]");
}

TEST_F(TinySnbUpdateTest, SetNodeListOfShortStringPropTest) {
    conn->query("MATCH (a:person) WHERE a.ID=0 SET a.usedNames=['intel','microsoft']");
    auto result = conn->query("MATCH (a:person) WHERE a.ID=0 RETURN a.usedNames");
    auto value = result->getNext()->getResultValue(0);
    ASSERT_EQ(value->to_string(), "[intel,microsoft]");
}

TEST_F(TinySnbUpdateTest, SetNodeListOfLongStringPropTest) {
    conn->query(
        "MATCH (a:person) WHERE a.ID=0 SET a.usedNames=['abcndwjbwesdsd','microsofthbbjuwgedsd']");
    auto result = conn->query("MATCH (a:person) WHERE a.ID=0 RETURN a.usedNames");
    auto value = result->getNext()->getResultValue(0);
    ASSERT_EQ(value->to_string(), "[abcndwjbwesdsd,microsofthbbjuwgedsd]");
}

TEST_F(TinySnbUpdateTest, SetNodeListofListPropTest) {
    conn->query("MATCH (a:person) WHERE a.ID=8 SET a.courseScoresPerTerm=[[10,20],[0,0,0]]");
    auto result = conn->query("MATCH (a:person) WHERE a.ID=8 RETURN a.courseScoresPerTerm");
    auto value = result->getNext()->getResultValue(0);
    ASSERT_EQ(value->to_string(), "[[10,20],[0,0,0]]");
}

TEST_F(TinySnbUpdateTest, SetVeryLongListErrorsTest) {
    conn->beginWriteTransaction();
    auto result =
        conn->query("MATCH (a:person) WHERE a.ID=0 SET a.fName=" + getStringExceedsOverflow());
    ASSERT_FALSE(result->isSuccess());
}

TEST_F(TinySnbUpdateTest, SetNodeIntervalPropTest) {
    conn->query("MATCH (a:person) WHERE a.ID=0 SET a.lastJobDuration=interval('1 years 1 days')");
    auto result = conn->query("MATCH (a:person) WHERE a.ID=0 RETURN a.lastJobDuration");
    string intervalStr = "1 years 1 days";
    ASSERT_EQ(result->getNext()->getResultValue(0)->getIntervalVal(),
        Interval::FromCString(intervalStr.c_str(), intervalStr.length()));
}

TEST_F(TinySnbUpdateTest, SetNodePropNullTest) {
    conn->query("MATCH (a:person) SET a.age=null");
    auto result = conn->query("MATCH (a:person) RETURN a.age");
    auto groundTruth = vector<string>{"", "", "", "", "", "", "", ""};
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, SetBothUnflatTest) {
    conn->query("MATCH (a:person) SET a.age=a.ID");
    auto result = conn->query("MATCH (a:person) WHERE a.ID < 4 RETURN a.ID, a.age");
    auto groundTruth = vector<string>{"0|0", "2|2", "3|3"};
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, SetFlatUnFlatTest) {
    conn->query("MATCH (a:person)-[:knows]->(b:person) WHERE a.ID=0 SET a.age=b.age");
    auto result = conn->query("MATCH (a:person) WHERE a.ID < 4 RETURN a.ID, a.age");
    auto groundTruth = vector<string>{"0|20", "2|30", "3|45"};
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, SetUnFlatFlatTest) {
    conn->query("MATCH (a:person)-[:knows]->(b:person) WHERE b.ID=2 AND a.ID = 0 SET b.age=a.age");
    auto result = conn->query("MATCH (a:person) WHERE a.ID < 4 RETURN a.ID, a.age");
    auto groundTruth = vector<string>{"0|35", "2|35", "3|45"};
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, SetTwoHopTest) {
    conn->query("MATCH (a:person)-[:knows]->(b:person)-[:knows]->(c:person) WHERE b.ID=0 AND "
                "c.fName = 'Bob' SET a.age=c.age");
    auto result = conn->query("MATCH (a:person) WHERE a.ID < 6 RETURN a.ID, a.age");
    auto groundTruth = vector<string>{"0|35", "2|30", "3|30", "5|30"};
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, SetTwoHopNullTest) {
    conn->query("MATCH (a:person)-[:knows]->(b:person)-[:knows]->(c:person) SET a.age=null");
    auto result = conn->query("MATCH (a:person) RETURN a.ID, a.age");
    auto groundTruth = vector<string>{"0|", "10|83", "2|", "3|", "5|", "7|20", "8|25", "9|40"};
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}
// TODO(Semih): Uncomment when enabling ad-hoc properties
// TEST_F(TinySnbUpdateTest, SetNodeUnstrIntPropTest) {
//    conn->query("MATCH (a:person) WHERE a.ID < 3 SET a.unstrInt64Prop=1");
//    auto result = conn->query("MATCH (a:person) WHERE a.ID < 6 RETURN a.ID,a.unstrInt64Prop");
//    auto groundTruth = vector<string>{"0|1", "2|1", "3|4541124", "5|"};
//    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
//}
//
// TEST_F(TinySnbUpdateTest, SetNodeUnstrDatePropTest) {
//    conn->query("MATCH (a:person) WHERE a.ID < 3 SET a.unstrDateProp1=date('2022-10-10')");
//    auto result = conn->query("MATCH (a:person) WHERE a.ID < 6 RETURN a.ID,a.unstrDateProp1");
//    auto groundTruth = vector<string>{"0|2022-10-10", "2|2022-10-10", "3|1950-01-01", "5|"};
//    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
//}
//
// TEST_F(TinySnbUpdateTest, SetNodeUnstrIntervalPropTest) {
//    conn->query("MATCH (a:person) WHERE a.ID < 3 SET a.unstrIntervalProp=interval('2 years')");
//    auto result = conn->query("MATCH (a:person) WHERE a.ID < 6 RETURN a.ID,a.unstrIntervalProp");
//    auto groundTruth = vector<string>{"0|2 years", "2|2 years", "3|", "5|"};
//    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
//}
//
// TEST_F(TinySnbUpdateTest, SetNodeUnstBoolPropTest) {
//    conn->query("MATCH (a:person) WHERE a.ID < 4 SET a.unstrBoolProp1=True");
//    auto result = conn->query("MATCH (a:person) WHERE a.ID < 6 RETURN a.ID,a.unstrBoolProp1");
//    auto groundTruth = vector<string>{"0|True", "2|True", "3|True", "5|False"};
//    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
//}
//
// TEST_F(TinySnbUpdateTest, SetNodeUnstrShortStringPropTest) {
//    conn->query("MATCH (a:person) WHERE a.ID < 3 SET a.label1='abcd'");
//    auto result = conn->query("MATCH (a:person) WHERE a.ID < 6 RETURN a.ID,a.label1");
//    auto groundTruth = vector<string>{"0|abcd", "2|abcd", "3|", "5|good"};
//    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
//}
//
// TEST_F(TinySnbUpdateTest, SetNodeUnstrLongStringPropTest) {
//    conn->query("MATCH (a:person) WHERE a.ID < 3 SET a.label1='abcdefghijklmnopqrstuvwxyz'");
//    auto result = conn->query("MATCH (a:person) WHERE a.ID < 6 RETURN a.ID,a.label1");
//    auto groundTruth = vector<string>{
//        "0|abcdefghijklmnopqrstuvwxyz", "2|abcdefghijklmnopqrstuvwxyz", "3|", "5|good"};
//    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
//}
//
// TEST_F(TinySnbUpdateTest, SetNodeUnstLongStringErrorTest) {
//    //    conn->query(
//    //        "MATCH (a:person) WHERE a.ID < 3 SET a.label1='" + getStringExceedsOverflow() +
//    "'");
//    //    auto result = conn->query("MATCH (a:person) WHERE a.ID < 6 RETURN a.ID,a.label1");
//    //    auto groundTruth = vector<string>{"0|", "2|", "3|4541124", "5|"};
//    //    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
//    //    ASSERT_FALSE(result->isSuccess());
//}
//
// TEST_F(TinySnbUpdateTest, SetNodeUnstrPropNullTest) {
//    conn->query("MATCH (a:person) WHERE a.ID < 3 SET a.unstrInt64Prop=null");
//    auto result = conn->query("MATCH (a:person) WHERE a.ID < 6 RETURN a.ID,a.unstrInt64Prop");
//    auto groundTruth = vector<string>{"0|", "2|", "3|4541124", "5|"};
//    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
//}
//
// TEST_F(TinySnbUpdateTest, SetNodeUnstrPropFromSameNodeTest) {
//    conn->query("MATCH (a:person) SET a.label1=a.label2");
//    auto result = conn->query("MATCH (a:person) WHERE a.ID < 6 RETURN a.ID, a.label1");
//    auto groundTruth = vector<string>{"0|excellent", "2|excellent", "3|", "5|excellent"};
//    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
//}
//
// TEST_F(TinySnbUpdateTest, SetNodeUnstrProp1HopTest) {
//    conn->query("MATCH (a:person)-[:knows]->(b:person) WHERE b.ID=0 SET a.label1=b.label2");
//    auto result = conn->query("MATCH (a:person) WHERE a.ID < 6 RETURN a.ID, a.label1");
//    auto groundTruth = vector<string>{"0|good", "2|excellent", "3|excellent", "5|excellent"};
//    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
//}
//
// TEST_F(TinySnbUpdateTest, SetNodeMixedPropTest) {
//    conn->query("MATCH (a:person) WHERE a.ID= 0 SET a.label1='abcd', a.fName='A'");
//    auto result = conn->query("MATCH (a:person) WHERE a.ID < 3 RETURN a.ID,a.fName,a.label1");
//    auto groundTruth = vector<string>{"0|A|abcd", "2|Bob|"};
//    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
//}

// Create clause test

TEST_F(TinySnbUpdateTest, InsertNodeWithoutPrimaryKeyTest) {
    auto result = conn->query("CREATE (a:person {isWorker:true});");
    ASSERT_STREQ("Binder exception: Create node a expects primary key ID as input.",
        result->getErrorMessage().c_str());
}

TEST_F(TinySnbUpdateTest, InsertNodeWithBoolIntDoubleTest) {
    conn->query("CREATE (:person {ID:80, isWorker:true,age:22,eyeSight:1.1});");
    auto result = conn->query("MATCH (a:person) WHERE a.ID > 8 "
                              "RETURN a.ID, a.gender,a.isStudent, a.isWorker, a.age, a.eyeSight");
    auto groundTruth = vector<string>{
        "10|2|False|True|83|4.900000", "80|||True|22|1.100000", "9|2|False|False|40|4.900000"};
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, InsertNodeWithDateIntervalTest) {
    conn->query("CREATE (:person {ID:32, birthdate:date('1997-03-22'), lastJobDuration:interval('2 "
                "years')});");
    auto result = conn->query("MATCH (a:person) WHERE a.ID > 8 "
                              "RETURN a.ID, a.birthdate,a.lastJobDuration");
    auto groundTruth = vector<string>{"10|1990-11-27|3 years 2 days 13:02:00",
        "32|1997-03-22|2 years", "9|1980-10-26|10 years 5 months 13:00:00.000024"};
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, InsertNodeWithStringTest) {
    conn->query("CREATE (:person {ID:32, fName:'A'}), (:person {ID:33, fName:'BCD'}), (:person "
                "{ID:34, fName:'this is a long name'});");
    auto result = conn->query("MATCH (a:person) WHERE a.ID > 8 RETURN a.ID, a.fName");
    auto groundTruth = vector<string>{"10|Hubert Blaine Wolfeschlegelsteinhausenbergerdorff",
        "32|A", "33|BCD", "34|this is a long name", "9|Greg"};
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, InsertNodeWithListTest) {
    conn->query(
        "CREATE (:person {ID:11, workedHours:[1,2,3], usedNames:['A', 'this is a long name']});");
    auto result = conn->query("MATCH (a:person) WHERE a.ID > 8 "
                              "RETURN a.ID, a.workedHours,a.usedNames");
    auto groundTruth = vector<string>{"10|[10,11,12,3,4,5,6,7]|[Ad,De,Hi,Kye,Orlan]",
        "11|[1,2,3]|[A,this is a long name]", "9|[1]|[Grad]"};
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, InsertNodeWithMixedLabelTest) {
    conn->query("CREATE (:person {ID:32, fName:'A'}), (:organisation {ID:33, orgCode:144});");
    auto pResult = conn->query("MATCH (a:person) WHERE a.ID > 30 RETURN a.ID, a.fName");
    auto pGroundTruth = vector<string>{"32|A"};
    ASSERT_EQ(TestHelper::convertResultToString(*pResult), pGroundTruth);
    auto oResult = conn->query("MATCH (a:organisation) RETURN a.ID, a.orgCode");
    auto oGroundTruth = vector<string>{"1|325", "33|144", "4|934", "6|824"};
    ASSERT_EQ(TestHelper::convertResultToString(*oResult), oGroundTruth);
}

// TODO(Semih): Uncomment when enabling ad-hoc properties
// TEST_F(TinySnbUpdateTest, InsertNodeWithUnstrTest) {
//    conn->query("CREATE (:person {ID:11, label1:'a', label2:'abcdefghijklmn'});");
//    auto result = conn->query(
//        "MATCH (a:person) WHERE a.ID > 5 RETURN a.ID, a.label1, a.label2, a.unstrDateProp1");
//    auto groundTruth = vector<string>{
//        "10||good|", "11|a|abcdefghijklmn|", "7|||", "8|excellent|excellent|", "9|good||"};
//    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
//}

TEST_F(TinySnbUpdateTest, InsertNodeAfterMatchListTest) {
    conn->query("MATCH (a:person) CREATE (:person {ID:a.ID+11, age:a.age*2});");
    auto result = conn->query("MATCH (a:person) RETURN a.ID, a.fName,a.age");
    auto groundTruth =
        vector<string>{"0|Alice|35", "10|Hubert Blaine Wolfeschlegelsteinhausenbergerdorff|83",
            "11||70", "13||60", "14||90", "16||40", "18||40", "19||50", "20||80", "21||166",
            "2|Bob|30", "3|Carol|45", "5|Dan|20", "7|Elizabeth|20", "8|Farooq|25", "9|Greg|40"};
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, InsertSingleNToNRelTest) {
    conn->query(
        "MATCH (a:person), (b:person) WHERE a.ID = 9 AND b.ID = 10 "
        "CREATE (a)-[:knows {meetTime:timestamp('1976-12-23 11:21:42'), validInterval:interval('2 "
        "years'), comments:['A', 'k'], date:date('1997-03-22')}]->(b);");
    auto groundTruth = vector<string>{"9|10|1997-03-22|1976-12-23 11:21:42|2 years|[A,k]|40"};
    auto result =
        conn->query("MATCH (a:person)-[e:knows]->(b:person) WHERE a.ID > 8 RETURN a.ID, b.ID, e");
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, InsertSingleNTo1RelTest) {
    // insert studyAt edge between Greg and CsWork
    conn->query("MATCH (a:person), (b:organisation) WHERE a.ID = 9 AND b.orgCode = 934 "
                "CREATE (a)-[:studyAt {year:2022}]->(b);");
    auto groundTruth =
        vector<string>{"8|325|2020|[awndsnjwejwen,isuhuwennjnuhuhuwewe]|16", "9|934|2022||40"};
    auto result = conn->query(
        "MATCH (a:person)-[e:studyAt]->(b:organisation) WHERE a.ID > 5 RETURN a.ID, b.orgCode, e");
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, InsertRepeatedNToNRelTest) {
    conn->query("MATCH (a:person), (b:person) WHERE a.ID = 7 AND b.ID = 8"
                "CREATE (a)-[:knows {validInterval:interval('3 years')}]->(b);");
    auto groundTruth = vector<string>{"8|00:47:58", "8|3 years", "9|00:47:58"};
    auto result = conn->query(
        "MATCH (a:person)-[e:knows]->(b:person) WHERE a.ID=7 RETURN b.ID, e.validInterval");
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, InsertMixedRelTest) {
    conn->query(
        "MATCH (a:person), (b:person), (c:organisation) WHERE a.ID = 0 AND b.ID = 9 AND c.ID = 4 "
        "CREATE (b)-[:studyAt]->(c), (a)<-[:knows]-(b)");
    auto groundTruth = vector<string>{"9"};
    auto result = conn->query(
        "MATCH (a:person)-[:knows]->(b:person)-[:studyAt]->(c:organisation) RETURN COUNT(*)");
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, InsertMultipleRelsTest) {
    conn->query("MATCH (a:person)-[:knows]->(b:person) WHERE a.ID = 7 "
                "CREATE (a)<-[:knows]-(b);");
    auto groundTruth = vector<string>{"7|8|12", "7|9|13", "8|7|40", "9|7|41"};
    auto result = conn->query("MATCH (a:person)-[e:knows]->(b:person) WHERE a.ID > 6 RETURN a.ID, "
                              "b.ID, e._id");
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, InsertNodeAndRelTest) {
    conn->query(
        "CREATE (a:person {ID:100})-[:knows {date:date('1997-03-22')}]->(b:person {ID:202})");
    auto groundTruth = vector<string>{"100|202|1997-03-22"};
    auto result = conn->query("MATCH (a:person)-[e:knows]->(b:person) WHERE a.ID > 50 RETURN a.ID, "
                              "b.ID, e.date");
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

TEST_F(TinySnbUpdateTest, InsertNodeAndRelTest2) {
    conn->query(
        "CREATE (c:organisation {ID:50})<-[:workAt]-(a:person {ID:100}), (a)-[:studyAt]->(c)");
    auto groundTruth = vector<string>{"100|50|41|40"};
    auto result = conn->query(
        "MATCH (a:person)-[e1:studyAt]->(b:organisation), (a)-[e2:workAt]->(b) RETURN a.ID, "
        "b.ID, e1._id, e2._id");
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}

// Delete clause test

TEST_F(TinySnbUpdateTest, DeleteNodeWithEdgeErrorTest) {
    auto result = conn->query("MATCH (a:person) WHERE a.ID = 0 DELETE a");
    ASSERT_FALSE(result->isSuccess());
}

TEST_F(TinySnbUpdateTest, DeleteNodeAfterInsertTest) { // test reset of property columns
    conn->query("CREATE (a:person {ID:100, fName:'Xiyang', age:26})");
    conn->query("MATCH (a:person) WHERE a.ID = 100 DELETE a;");
    conn->query("CREATE (a:person {ID:101})");
    auto result = conn->query("MATCH (a:person) WHERE a.ID > 10 RETURN a.ID, a.fName, a.age");
    auto groundTruth = vector<string>{"101||"};
    ASSERT_EQ(TestHelper::convertResultToString(*result), groundTruth);
}
