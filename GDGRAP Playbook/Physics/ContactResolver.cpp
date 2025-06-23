#include "ContactResolver.h"

void ContactResolver::ResolveContacts(std::list<ParticleContact*> contacts, float time)
{
    unsigned int resolveCount = 0;
    while (resolveCount < max_iteration)
    {
        // Find the contact with the largest closing velocity (most negative separating speed)
        // or with penetration (depth > 0)
        float minSepSpeed = std::numeric_limits<float>::max();
        ParticleContact* contactToResolve = nullptr;

        for (auto contact : contacts)
        {
            float sepSpeed = contact->GetSeparatingSpeed();
            if (sepSpeed < minSepSpeed || contact->depth > 0.0f)
            {
                minSepSpeed = sepSpeed;
                contactToResolve = contact;
            }
        }

        // If no contact needs resolving, break
        if (!contactToResolve) break;

        // If the best contact is already separating and not penetrating, we're done
        if (contactToResolve->GetSeparatingSpeed() >= 0.0f && contactToResolve->depth <= 0.0f)
            break;

        // Resolve this contact
        contactToResolve->Resolve(time);

        // Increment resolve count
        ++resolveCount;
    }
}