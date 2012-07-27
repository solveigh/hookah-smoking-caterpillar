//
// Generated file, do not edit! Created by opp_msgc 4.2 from Packet.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "Packet_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}



namespace qlib {

WRPacket_Base::WRPacket_Base(const char *name, int kind) : cPacket(name,kind)
{
    this->priority_var = 0;
    this->totalQueueingTime_var = 0;
    this->totalServiceTime_var = 0;
    this->totalDelayTime_var = 0;
    this->queueCount_var = 0;
    this->delayCount_var = 0;
    this->generation_var = 0;
    this->operationCounter_var = 0;
}

WRPacket_Base::WRPacket_Base(const WRPacket_Base& other) : cPacket(other)
{
    copy(other);
}

WRPacket_Base::~WRPacket_Base()
{
}

WRPacket_Base& WRPacket_Base::operator=(const WRPacket_Base& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void WRPacket_Base::copy(const WRPacket_Base& other)
{
    this->priority_var = other.priority_var;
    this->totalQueueingTime_var = other.totalQueueingTime_var;
    this->totalServiceTime_var = other.totalServiceTime_var;
    this->totalDelayTime_var = other.totalDelayTime_var;
    this->queueCount_var = other.queueCount_var;
    this->delayCount_var = other.delayCount_var;
    this->generation_var = other.generation_var;
    this->operationCounter_var = other.operationCounter_var;
}

void WRPacket_Base::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->priority_var);
    doPacking(b,this->totalQueueingTime_var);
    doPacking(b,this->totalServiceTime_var);
    doPacking(b,this->totalDelayTime_var);
    doPacking(b,this->queueCount_var);
    doPacking(b,this->delayCount_var);
    doPacking(b,this->generation_var);
    doPacking(b,this->operationCounter_var);
}

void WRPacket_Base::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->priority_var);
    doUnpacking(b,this->totalQueueingTime_var);
    doUnpacking(b,this->totalServiceTime_var);
    doUnpacking(b,this->totalDelayTime_var);
    doUnpacking(b,this->queueCount_var);
    doUnpacking(b,this->delayCount_var);
    doUnpacking(b,this->generation_var);
    doUnpacking(b,this->operationCounter_var);
}

int WRPacket_Base::getPriority() const
{
    return priority_var;
}

void WRPacket_Base::setPriority(int priority)
{
    this->priority_var = priority;
}

simtime_t WRPacket_Base::getTotalQueueingTime() const
{
    return totalQueueingTime_var;
}

void WRPacket_Base::setTotalQueueingTime(simtime_t totalQueueingTime)
{
    this->totalQueueingTime_var = totalQueueingTime;
}

simtime_t WRPacket_Base::getTotalServiceTime() const
{
    return totalServiceTime_var;
}

void WRPacket_Base::setTotalServiceTime(simtime_t totalServiceTime)
{
    this->totalServiceTime_var = totalServiceTime;
}

simtime_t WRPacket_Base::getTotalDelayTime() const
{
    return totalDelayTime_var;
}

void WRPacket_Base::setTotalDelayTime(simtime_t totalDelayTime)
{
    this->totalDelayTime_var = totalDelayTime;
}

int WRPacket_Base::getQueueCount() const
{
    return queueCount_var;
}

void WRPacket_Base::setQueueCount(int queueCount)
{
    this->queueCount_var = queueCount;
}

int WRPacket_Base::getDelayCount() const
{
    return delayCount_var;
}

void WRPacket_Base::setDelayCount(int delayCount)
{
    this->delayCount_var = delayCount;
}

int WRPacket_Base::getGeneration() const
{
    return generation_var;
}

void WRPacket_Base::setGeneration(int generation)
{
    this->generation_var = generation;
}

int WRPacket_Base::getOperationCounter() const
{
    return operationCounter_var;
}

void WRPacket_Base::setOperationCounter(int operationCounter)
{
    this->operationCounter_var = operationCounter;
}

class WRPacketDescriptor : public cClassDescriptor
{
  public:
    WRPacketDescriptor();
    virtual ~WRPacketDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(WRPacketDescriptor);

WRPacketDescriptor::WRPacketDescriptor() : cClassDescriptor("qlib::WRPacket", "cPacket")
{
}

WRPacketDescriptor::~WRPacketDescriptor()
{
}

bool WRPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<WRPacket_Base *>(obj)!=NULL;
}

const char *WRPacketDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int WRPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 8+basedesc->getFieldCount(object) : 8;
}

unsigned int WRPacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<8) ? fieldTypeFlags[field] : 0;
}

const char *WRPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "priority",
        "totalQueueingTime",
        "totalServiceTime",
        "totalDelayTime",
        "queueCount",
        "delayCount",
        "generation",
        "operationCounter",
    };
    return (field>=0 && field<8) ? fieldNames[field] : NULL;
}

int WRPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='p' && strcmp(fieldName, "priority")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "totalQueueingTime")==0) return base+1;
    if (fieldName[0]=='t' && strcmp(fieldName, "totalServiceTime")==0) return base+2;
    if (fieldName[0]=='t' && strcmp(fieldName, "totalDelayTime")==0) return base+3;
    if (fieldName[0]=='q' && strcmp(fieldName, "queueCount")==0) return base+4;
    if (fieldName[0]=='d' && strcmp(fieldName, "delayCount")==0) return base+5;
    if (fieldName[0]=='g' && strcmp(fieldName, "generation")==0) return base+6;
    if (fieldName[0]=='o' && strcmp(fieldName, "operationCounter")==0) return base+7;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *WRPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "simtime_t",
        "simtime_t",
        "simtime_t",
        "int",
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<8) ? fieldTypeStrings[field] : NULL;
}

const char *WRPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int WRPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    WRPacket_Base *pp = (WRPacket_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string WRPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    WRPacket_Base *pp = (WRPacket_Base *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getPriority());
        case 1: return double2string(pp->getTotalQueueingTime());
        case 2: return double2string(pp->getTotalServiceTime());
        case 3: return double2string(pp->getTotalDelayTime());
        case 4: return long2string(pp->getQueueCount());
        case 5: return long2string(pp->getDelayCount());
        case 6: return long2string(pp->getGeneration());
        case 7: return long2string(pp->getOperationCounter());
        default: return "";
    }
}

bool WRPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    WRPacket_Base *pp = (WRPacket_Base *)object; (void)pp;
    switch (field) {
        case 0: pp->setPriority(string2long(value)); return true;
        case 1: pp->setTotalQueueingTime(string2double(value)); return true;
        case 2: pp->setTotalServiceTime(string2double(value)); return true;
        case 3: pp->setTotalDelayTime(string2double(value)); return true;
        case 4: pp->setQueueCount(string2long(value)); return true;
        case 5: pp->setDelayCount(string2long(value)); return true;
        case 6: pp->setGeneration(string2long(value)); return true;
        case 7: pp->setOperationCounter(string2long(value)); return true;
        default: return false;
    }
}

const char *WRPacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<8) ? fieldStructNames[field] : NULL;
}

void *WRPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    WRPacket_Base *pp = (WRPacket_Base *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

}; // end namespace qlib

