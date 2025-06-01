## TODO

* [ ] Collision manager to allow collision callbacks in Lua scripts
* [ ] Add script update hook
* [ ] Create design document
* [x] Consistent naming of `2D` classes & files (should be e.g. `water_2d_manager.h` for `Water2DManager`)
* [x] Fix blocked cell checks for `FlowField2D` (sampled directions can point into a blocked cell diagonally without correction)
* [ ] Bee behavior: snap current velocity based on blocked cells so flight response can be reimplemented
* [ ] Create separate layer for squirrel tail if there's an obstruction behind so it doesn't clip too much
* [ ] Instead of passing managers to script factory, pull the necessary global data from the `Global` component in the registry

## Four Pillars

### 1. Crafting

The core gameplay loop involves discovering celestial bodies (planets, asteroids, moons) and consuming
their resources through crafting, ship-building, or commerce to build an expanding interstellar corporation.

### 2. Logistics

To support the main gameplay loop will be a series of logistics operations such as maintaining a fleet of
vessels to support ground and space operations. This can be done by building them and hiring a crew, or
by contracting an existing NSIC or freelance crew.

### 3. Exploration

Discovering new worlds is part of the adventure, and can open up many new opportunities.

### 4. Collection

## Terminology

* NSIC (Non-syndicated Intersteller Corporation): Designation for entities that operate in multiple star systems
  regardless of their industry (military, manufacturing, extraction, aerospace, etc.)
* SJRN (Star-to-star Jump Relay Network): Commonly pronounced as "sojourn", the SJRN is a complex network of
  relay sites that aims to reduce the transmission time of information across the galaxy by utilizing hyperspeed
  jumps to sidestep the limitations imposed by the speed of light.
* AFI (Antimatter Field Inverter): A special class of hyperspeed engine that allows faster-than-light travel
  by creating strong opposing gravitational forces around the vessel which essentially pull it through space.
  This is an instantaneous form of travel but can be dangerous since the vessel technically occupies all points
  along its path of travel at the same time, and any matter it comes in contact with results in a relatively
  massive release of gravitational and thermal energy. For most particles and dust, this is a non-issue, and
  standard heat shielding protects against this, but for larger debris and celestial bodies, a collision is
  catastrophic. AFI travel is limited to jumping lanes maintained by the ISS. In the past, it had been used as an extremely
  expensive yet effective weapon by launching projectiles through a high-density medium at a target, resulting
  in cataclysmic destruction with relatively low risk at its source. AFI travel outside of these lanes is extremely
  dangerous and is ubiquitously considered a serious crime.
* ISS (Intersteller Spacefaring Syndicate): The only regulatory body fully observed by all major factions within the
  galaxy. They maintain the AFI jump lane networks that ensure safe interstellar travel, as well as SJRN relays.
* SSE (Subspace Engine): A hyperspeed engine class that allows safe faster-than-light travel. Collisions are never a
  concern since the vessel travels though an ephemeral layer of subspace to reach its target. On average, subspace
  travel is one hundred times faster than light.

## Brainstorming

### Celestial traits

These traits affect types of resource acquisition (e.g. robots or spacesuits required for extraction on planets
with no atmosphere). Others affect the efficiency, e.g. low gravity reduces fuel costs for transport but requires
workers exercise and undergo physical therapy to maintain health.

* Atmosphere
* Magnetosphere
* Flora
* Fauna
* Gravity
* Weather
* Toxicity

### Interstellar Freight

* Various payment strategies to help with currency management: on-acquisition, on-delivery, in-transit

### Interstellar Banking Services

* Offers limited credit to help keep operations intact

### Currency management

One idea would be to have multiple currencies (for multiple governing bodies). Alternatively, currency transfer
can be limited by the speed of light, so currency generated thousands of light years away is unusable until you
retrieve it via SJRN. This can have profound implications for interstellar operations, requiring careful management
of supply chains and commerce to ensure operations don't fall apart while you are in a distant galaxy.

### Galactic simulation

An interesting mechanic would be utilizing the concept of relativity in your corporations operations. For example,
orbiting a black hole to slow your timeline relative to a mining operation on a distant planet to increase its
productivity. Associated risks could be contract violations due to missed payments if your supply chain doesn't
generate proper funds without management during your period away, or not having the proper equipment to maintain
safe orbit around the black hole.

## Credits

### Xantoor

For game design guidance and direction.
