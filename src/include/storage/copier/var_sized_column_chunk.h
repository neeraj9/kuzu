#pragma once

#include "storage/copier/column_chunk.h"
#include "storage/storage_structure/in_mem_file.h"

namespace kuzu {
namespace storage {

class VarSizedColumnChunk : public ColumnChunk {
public:
    VarSizedColumnChunk(common::LogicalType dataType, common::CopyDescription* copyDescription);

    void resetToEmpty() final;
    void append(
        arrow::Array* array, common::offset_t startPosInChunk, uint32_t numValuesToAppend) final;
    void append(ColumnChunk* other, common::offset_t startPosInOtherChunk,
        common::offset_t startPosInChunk, uint32_t numValuesToAppend) final;
    common::page_idx_t flushBuffer(BMFileHandle* dataFH, common::page_idx_t startPageIdx) final;

    template<typename T>
    void setValueFromString(const char* value, uint64_t length, uint64_t pos) {
        throw common::NotImplementedException("VarSizedColumnChunk::setValueFromString");
    }
    template<typename T>
    T getValue(common::offset_t pos) const {
        throw common::NotImplementedException("VarSizedColumnChunk::getValue");
    }

protected:
    inline common::page_idx_t getNumPagesForBuffer() const final {
        auto numPagesForOffsets = ColumnChunk::getNumPagesForBuffer();
        return numPagesForOffsets + overflowFile->getNumPages();
    }

private:
    template<typename T>
    void templateCopyVarSizedValuesFromString(
        arrow::Array* array, common::offset_t startPosInChunk, uint32_t numValuesToAppend);
    void copyValuesFromVarList(
        arrow::Array* array, common::offset_t startPosInChunk, uint32_t numValuesToAppend);

    void appendStringColumnChunk(VarSizedColumnChunk* other, common::offset_t startPosInOtherChunk,
        common::offset_t startPosInChunk, uint32_t numValuesToAppend);
    void appendVarListColumnChunk(VarSizedColumnChunk* other, common::offset_t startPosInOtherChunk,
        common::offset_t startPosInChunk, uint32_t numValuesToAppend);

private:
    std::unique_ptr<InMemOverflowFile> overflowFile;
    PageByteCursor overflowCursor;
};

// BOOL
template<>
void VarSizedColumnChunk::setValueFromString<common::blob_t>(
    const char* value, uint64_t length, uint64_t pos);
// STRING
template<>
void VarSizedColumnChunk::setValueFromString<common::ku_string_t>(
    const char* value, uint64_t length, uint64_t pos);
// VAR_LIST
template<>
void VarSizedColumnChunk::setValueFromString<common::ku_list_t>(
    const char* value, uint64_t length, uint64_t pos);

// STRING
template<>
std::string VarSizedColumnChunk::getValue<std::string>(common::offset_t pos) const;

} // namespace storage
} // namespace kuzu