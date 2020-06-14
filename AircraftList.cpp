#include "AircraftList.h"

static auto now() {
    return std::chrono::high_resolution_clock::now();
}


Aircraft *AircraftList::find(uint32_t addr) {
    Aircraft *p = head;

    while(p) {
        if (p->addr == addr) return (p);
        p = p->next;
    }
    return (nullptr);
}
    
	//instead of this, net_io should call this class directly to update info
void AircraftList::update(Modes *modes) {
    struct aircraft *a = modes->aircrafts;

    Aircraft *p = head;

    while(p) {
        p->live = 0;
        p = p->next;
    }

    while(a) {

        p = find(a->addr);
        if (!p) {
            p = new Aircraft(a->addr);
            p->next = head;       
            head = p;      
        } else {
            p->prev_seen = p->seen;
        }

        p->live = 1;

        if(p->seen == a->seen) {
            a = a->next;
            continue;
        }

        p->seen = a->seen;            
        p->msSeen = now();

        if((p->seen - p->prev_seen) > 0) {
                p->messageRate = 1.0 / (double)(p->seen - p->prev_seen);
        }

        memcpy(p->flight, a->flight, sizeof(p->flight));
        memcpy(p->signalLevel, a->signalLevel, sizeof(p->signalLevel));


        if(p->seenLatLon == a->seenLatLon) {
            a = a->next;
            continue;
        }

        p->msSeenLatLon = now();

        p->seenLatLon = a->seenLatLon;

        p->altitude = a->altitude;
        p->speed =  a->speed;          
        p->track = a->track;                  

        p->vert_rate = a->vert_rate;    

        if(p->lon == 0) {
            p->created = now();
        }
        p->lon = a->lon;
        p->lat = a->lat;

        p->lonHistory.push_back(p->lon);
        p->latHistory.push_back(p->lat);
        p->headingHistory.push_back(p->track);
        p->timestampHistory.push_back(p->msSeenLatLon);
        
        a = a->next;
    }

    p = head;
    Aircraft *prev = nullptr;

    while(p) {
        if(!p->live) {
            if (!prev) {
                head = p->next; 
                delete(p); 
                p = head; 
            } else {
                prev->next = p->next; 
                delete(p); 
                p = prev->next;
            }
        } else {
            prev = p;
            p = p->next;
        }
    }
}

AircraftList::AircraftList() {
    head = nullptr;
}

AircraftList::~AircraftList() {
    while(head != nullptr) {
        Aircraft *temp = head;
        head  = head->next;
        delete(temp);
    }
}
