//
// Created by mad on 12/14/15.
//
#include <new>
#include <cstring>
#include <sys/mman.h>
#include "Debug.h"
#include "Util.h"
#include "SequenceLookup.h"

SequenceLookup::SequenceLookup(size_t dbSize, size_t entrySize)
        : sequenceCount(dbSize), dataSize(entrySize), currentIndex(0), currentOffset(0), externalData(false) {
    data = new(std::nothrow) char[dataSize + 1];
    Util::checkAllocation(data, "Could not allocate data memory in SequenceLookup");

    offsets = new(std::nothrow) size_t[sequenceCount + 1];
    Util::checkAllocation(offsets, "Could not allocate offsets memory in SequenceLookup");
    offsets[sequenceCount] = dataSize;
}

SequenceLookup::SequenceLookup(size_t dbSize)
        : sequenceCount(dbSize), data(NULL), dataSize(0), offsets(NULL), currentIndex(0), currentOffset(0), externalData(true) {
}

SequenceLookup::~SequenceLookup() {
    if(externalData == false){
        delete[] data;
        delete[] offsets;
    }
}

void SequenceLookup::addSequence(int *seq, int L, size_t index, size_t offset){
    offsets[index] = offset;
    for(int pos = 0; pos < L; pos++){
        unsigned char aa = seq[pos];
        data[offset + pos] = aa;
    }
}

void SequenceLookup::addSequence(Sequence *seq) {
    addSequence(seq->int_sequence, seq->L, currentIndex, currentOffset);
    currentIndex = currentIndex + 1;
    currentOffset = currentOffset + seq->L;
}

std::pair<const unsigned char *, const unsigned int> SequenceLookup::getSequence(size_t id) {
    ptrdiff_t N = offsets[id + 1] - offsets[id];
    char *p = data + offsets[id];
    return std::pair<const unsigned char *, const unsigned int>(reinterpret_cast<const unsigned char*>(p), static_cast<unsigned int>(N));
}

const char *SequenceLookup::getData() {
    return data;
}

int64_t SequenceLookup::getDataSize() {
    return dataSize;
}

size_t *SequenceLookup::getOffsets() {
    return offsets;
}

size_t SequenceLookup::getSequenceCount() {
    return sequenceCount;
}

void SequenceLookup::initLookupByExternalData(char *seqData, size_t seqDataSize, size_t *seqOffsets) {
    // copy data to data element
    data = seqData;
    dataSize = seqDataSize;
    offsets = seqOffsets;
}
