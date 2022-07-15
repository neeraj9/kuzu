#pragma once

#include "src/loader/in_mem_builder/include/in_mem_structures_builder.h"
#include "src/storage/index/include/in_mem_hash_index.h"

namespace graphflow {
namespace loader {

class InMemNodeBuilder : public InMemStructuresBuilder {

public:
    InMemNodeBuilder(label_t nodeLabel, const NodeFileDescription& fileDescription,
        string outputDirectory, TaskScheduler& taskScheduler, Catalog& catalog,
        LoaderProgressBar* progressBar);

    ~InMemNodeBuilder() override = default;

    inline uint64_t getNumNodes() { return numNodes; }
    void load();
    void saveToFile() override;

private:
    void initializeColumnsAndList();
    vector<string> countLinesPerBlockAndParseUnstrPropertyNames(uint64_t numStructuredProperties);
    void populateColumnsAndCountUnstrPropertyListSizes(uint64_t numNodes);
    void calcUnstrListsHeadersAndMetadata();
    void populateUnstrPropertyLists();

    static void calcLengthOfUnstrPropertyLists(
        CSVReader& reader, node_offset_t nodeOffset, InMemUnstructuredLists* unstrPropertyLists);
    static void putUnstrPropsOfALineToLists(CSVReader& reader, node_offset_t nodeOffset,
        PageByteCursor& stringOvfPagesCursor,
        unordered_map<string, uint64_t>& unstrPropertiesNameToIdMap,
        InMemUnstructuredLists* unstrPropertyLists);

    static void putPropsOfLineIntoColumns(vector<unique_ptr<InMemColumn>>& columns,
        const vector<Property>& properties, vector<PageByteCursor>& overflowCursors,
        CSVReader& reader, uint64_t nodeOffset);
    template<DataTypeID DT>
    static void addIDsToIndex(InMemColumn* column, InMemHashIndex* hashIndex,
        node_offset_t startOffset, uint64_t numValues);
    static void populateIDIndex(InMemColumn* column, InMemHashIndex* IDIndex,
        node_offset_t startOffset, uint64_t numValues);

    // Concurrent tasks.
    static void populateColumnsAndCountUnstrPropertyListSizesTask(uint64_t IDColumnIdx,
        uint64_t blockId, uint64_t offsetStart, InMemHashIndex* IDIndex, InMemNodeBuilder* builder);
    static void populateUnstrPropertyListsTask(
        uint64_t blockId, node_offset_t offsetStart, InMemNodeBuilder* builder);

private:
    DataType IDType;
    uint64_t numNodes;
    vector<unique_ptr<InMemColumn>> structuredColumns;
    unique_ptr<InMemUnstructuredLists> unstrPropertyLists;
};

} // namespace loader
} // namespace graphflow