#ifndef PETMELEEHIT_H
#define PETMELEEHIT_H

#include "Event.h"

class Pet;

class PetMeleeHit: public Event {
public:
    PetMeleeHit(Pet* pet, const int iteration, const double timestamp);

    void act() override;

private:
    Pet* pet;
    int iteration;
};

#endif // PETMELEEHIT_H
