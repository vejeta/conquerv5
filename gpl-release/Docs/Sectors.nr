.CH "Sector Information"
.CS "The Sector"
.PP
One of the most basic foundations of the world of Conquer, is the
concept of the "sector".  A sector is the smallest unit of land area
upon which the conquer map is based.  For reference, I suppose that a
sector could be thought of a being, say 100 to 250 square miles in
area, and of regular dimensions.  When troops are moved, they must be
moved from one sector to another, with there never being any "in
between"; thus a specific unit may be thought of as residing within a
specific sector.
.PP
A sector is described as consisting of a certain type of terrain (such
as mountains or hills, etc.), and being covered by a certain class of
vegetation (such as wood, swamp, or light vegetation).  In addition to
being characterized by the general geographical and ecological layout,
each sector may also possess special characteristics (such as jewel or
metal deposits) which can make it more desirable to the nations in the
world.
.PP
As might be expected, nations will be vying for control and possession
of the sectors within the world.  Taking control of a sector is
accomplished by having troops within the sector, without any other
nations claiming the sector or having their troops there to block the
claim to the land.  Once a sector is under the ownership of a nation,
that nation may then build farms, cities, or a variety of other
"designations" to which the land can be put to use.
.CE
.CS "Vegetation"
.PP
The vegetation of a sector is the greatest indicator of how much food
and wood is capable of being raised within the sector, and even
indicates how easily a unit may move through the sector.  Also, each
racial class will have a preference towards certain vegetations, being
better suited towards survival in such sectors.
.PP
The complete list of the twelve vegetation types is given below.  The
bracketed expression shows the default map character, food production
potential and wood production potential, respectively.
.IT
barren [`b', 4, 0] -- Such land is barely habitable, without any
potential for wood and only a small portion of food can be harvested
from this sector.  Elves actually find such sectors unsuitable for
farming.
.IT
desert [`.', 0, 0] -- A desert is a barren and inhospitable place, and
for most nations provides nothing of value.
.IT
forest [`f', 4, 8] -- A forest sector provides only a minimal supply
of food for most nations due to the over abundance of trees, which
cover most of the available land of the sector.
.IT
good [`g', 9, 2] -- A sector with good vegetation provides more food
producing potential than any other vegetation type, and also provides
a small amount of wood potential.
.IT
ice [`i', 0, 0] -- This inhospitable and harsh vegetation is not for
most nations, and therefore provides no food or wood resources.
.IT
jungle [`j', 0, 10] -- This harsh temperate vegetation is inhospitable
to most nations, but has an abundant wood supply if the nation can
live within it to harvest it.
.IT
light vegetation [`l', 6, 1] -- This vegetation indicates land capable
of supporting moderate food harvests and very light wood harvests.
.IT
none [`~', 0, 0] -- Only water sectors possess no vegetation.
.IT
swamp [`"', 0, 2] -- A sector with the vegetation designation of swamp
is deemed uninhabitable and is therefore not useful to most nations.
.IT
tundra [`,', 0, 0] -- This harsh icy vegetation is uninhabitable to
all nations, and provides no food or wood resources whatsoever.
.IT
volcano [`!', 0, 0] -- A volcano is a magnificent display of the power
of nature... creating a sector which is inaccessible to most everyone.
.IT
wood [`w', 7, 4] -- A sector whose vegetation is moderately wooded.
Wood sectors are suited for wood or food production, since the land is
both fertile and covered with a fair amount of trees.
.UT
.PP
Some of the uninhabitable vegetations may become usable once certain
magical powers are obtained.
.RF "Powers" "Magical Powers"
.CE
.CS "Elevation"
.PP
The terrain of a sector can play a significant role in game play.  The
difficulty of moving into a sector is a direct factor of the terrain
and vegetation of the sector.  Also, the attractiveness of the sector
to the population is affected by the terrain of the sector.  And
finally, and possibly most importantly, the defensive capabilities of
a sector are largely a result of the terrain.
.PP
The are only six elevation types, which range lowest to highest:
water, valley, clear, hill, mountain, peak.  A more descriptive list
is given below, with the affect that terrain has upon both food and
wood production listed within brackets next to the default map symbol.
.IT
water [`~', 0, 0] -- The terrain in this sector is non-existent, being
covered by water from a sea or lake.  This sector is inaccessible to
overland troops, but provides easy travel for naval or flying units.
.IT
valley [`v', +1, +1] -- The ruggedness of this terrain has caused the
contour of the land to be shaped such that passage must take place
through a valley area.  This provides ideal land for ambushes and
fortified defenses.  Movement is only slightly hindered in such
terrain.
.IT
flat [`-', +1, 0] -- A sector with the elevation of flat may be
considered generally level, and easy to move through, with little in
the way of offensive or defensive enhancement.
.IT
hill [`%', 0, 0] -- The terrain of a sector consisting of hills would
provide only minor adjustments to both attack and defense, but would
be of good benefit to fortified defenses in such a region.  The
movement is only slightly affected by this terrain.
.IT
mountain [`^', -1, -1] -- Mountainous terrain is considered very
beneficial to both fortified structures and defensive positions.  But,
such sectors are more difficult to move through.
.IT
peak [`#', -2, -2] -- Sectors consisting of mountain peaks are easily
accessible only to flying troops, and then just for passing through.
It is unlikely that a flying unit could survive landing upon such
harsh terrain.
.UT
.PP
Again, motion through sectors is governed by both vegetation and
elevation.
.RF "Movement" "Unit Relocation"
.CE
.CS "Tradegoods"
.PP
Within many, if not most, of the sectors of the world, there exists a
single "special" trait of the sector which can make the sector even
more powerful to a productive nation.  These "traits", or tradegoods,
provide the sector with a potential for harvesting rare minerals,
growing rare fruits, or even for the finding of magical artifacts.
.PP
There are a number of different classes of tradegoods, which each
class providing a different method of harvesting the tradegood, as
well as a different result for having such a tradegood harvested.
.PP
The various tradegood classes, and the method of using the tradegoods
within the class are:
.IT
None -- This class is an indication that no tradegoods are within the
sector.
.IT
Popularity -- These goods are harvested by farms cultivating the
crops, or in some cases, by just having civilians in the sector.  Such
materials increase the popularity of the national government.
.IT
Communications -- Building a supply center of at least the indicated
level provides the nation with the ability to make use of the
communication method within the sector.  Such methods improve the
communication range of both the supply center and the nation.
.IT
Fishing -- Building a fishing "Farm" within the sector gives the
nation the ability to harvest fish from nearby water sectors.  Thus,
fishing provides increased food production for the sector.
.IT
Farming -- The tools necessary to increase farming production are
available within the sector.  They provide added food production
potential and increase the farming ability of the nation.
.IT
Spoilrate -- Building a supply center within a sector with a tradegood
of this class decreases the rate at which food will spoil within that
sector.
.IT
Lumber -- A sector with added wood potential may be harvested using
lumberyards.
.IT
Knowledge -- Towns and Cities with such tradegoods add to the wealth
of knowledge in the nation.
.IT
Eat Rate -- Farm sectors can be used to harvest such valuable edibles.
Increasing the energy value of the food intact decreases the volume of
food the nation needs to consume.
.IT
Health -- Shrines and the priests within the shrines understand how to
make use of the herbs for the good of the nation.  Using such
tradegoods increases the health of the nation population.
.IT
Terror -- Cities with such items increase the terror and fear the
populace feels towards their government.
.IT
Magical -- Shrines built around such magical items increase the spell
casting ability of the nation.
.IT
Metals -- Metal mines are used to gather such raw resources and
provide metal ore for the nation.
.IT
Jewels -- Jewel mines dig the valuable minerals from the earth,
providing jewels for the nation.
.UT
.PP
A minimum number of civilians must occupy a sector for the tradegood
to be "harvested", and sometimes conditions must be met before a
tradegood is visible to a nation.  The next section lists the
tradegoods grouped by type.
.RF "Economics" "Economic Management"
.CE
.CS "The Individual Tradegoods"
.TB
    Popularity:  furs, wool, cloth, beer, wine, cannabis, poppy.

    Cummunications:  mules, horses, pigeons, griffons.

    Fishing:  pike, crayfish, crab, salmon, trout, shrip, lobster, flounder.

    Eat Rate:  barley, peas, wheat, dairy, rice, corn, sugar, fruit, honey.

    Spoilrate:  pottery, nails, salt, granite.

    Lumber:  bamboo, pine, oak, redwood, mahogany.

    Knowledge:  papyrus, drama, math, library, literature, law, philosophy.

    Farming:  oxen, yeoman, mulch, irrigation, rotation, plows, manure.

    Health:  sassafras, sulfa, poppy, foxglove, kannabis, bread mold,
       parsley, sage, rosemary, thyme, wolfsbane.

    Terror:  prison, curfews, torture, gallows, dungeon, pogrom.

    Magical:  icons, scrolls, altars, potions, mirrors, scepter, tomes,
       orbs, rings, staves, crystals.

    Metals:  nickel, copper, lead, tin, bronze, iron, bauxite, steel,
       titanium, mithral, iridium, adamantine.

    Jewels:  quartz, jade, turquoise, marble, sapphires, garnet, emeralds,
       corundum, silver, tourmaline, gold, opals, rubies, diamonds, platnum.
.TE
.PP
An even more detail listing of the tradegood values, workers needed,
and rarity of the various tradegoods is available in the online
documentation.
.RF "Tradegoods" "Online documentation: Tradegoods"
.CE
.CS "What are designations?"
.PP
Once a nation owns a sector, that nation may use it to produce various
goods and resources, or to construct fortifications, bridges, or even
canals.  The terrain and vegetation of a sector would be a key factor
in deciding how each sector should be put to use.  When a use for a
sector has been determined, that sector is given a "designation" which
will then indicate what the people in the sector are to do for a
living.
.PP
In order to make the most use of a sector, there are two classes of
designations:  major and minor.  The major designations may be thought
of as the "dedicated use" for the sector; the minor designations may
be thought of as the secondary constructions placed within the sector
to contribute to the major function of the sector.  Some examples of
major designations are:  farms, mines, and cities.  Some examples of
minor designations are:  roads, granaries, and churches.
.PP
It should be noted that the civilians of a nation will move in or out
of a sector based on what the major or minor designations are.  [After
all, would you want to work in an iron mine?]
.CE
.CS "The Major Designations"
.PP
As noted before, major designations may be thought of as the "use" to
which the sector is put.  Including "none", there are sixteen major
designations:
.IT
farm [`F'] -- A farm sector is dedicated to growing food, but is only
in the beginning stages of crop production.  Farm sectors become
fertile only during the transition between spring and summer.
.IT
fertile [`F'] -- A fertile sector is a farm sector whose crops are
starting to be productive.  A fertile sector will become fruitful when
the summer changes to fall.
.IT
fruitful [`F'] -- A fruitful farm sector is one whose crops are
available for harvest.  Only fertile can become fruitful.  Once fall
turns to winter, fruitful sectors will revert to farms.
.IT
metal mine [`m'] -- Metals such as iron and bronze are dug from the
ground in metal mines.  Only sectors containing discovered metal
deposits may become metal mines.
.IT
jewel mine [`$'] -- A jewel mine is much like a metal mine, but the
deposits to be mined are precious metals and gems, not metals.
.IT
lumberyard [`:'] -- The last production designation is the lumberyard.
This sector supplies wood to neighboring sectors.
.IT
shrine [`@'] -- A shrine sector can be a focus for magical energy.
National spell casting powers will increase if shrines posses magical
tradegoods.  Resting leaders in shrines increases recuperative powers
and spell casting from within a shrine is much easier than elsewhere.
.IT
bridge [`['] -- A bridge sector is useful to get troops over a water
sector which is separating two land sectors.
.IT
canal [`='] -- A canal sector is used to create a waterway between two
land separated water sectors.
.IT
wall [`|'] -- Walls were one of the great achievements of the Romans.
Such sectors provide added defense and prevent movement of enemy
troops unless they come through unimpeded.
.IT
cache [`&'] -- A sector with this designation acts as nothing more
than a storage site for materials.  Due to the nature of the cache, it
is not visible to other nations, acting as a hidden depot.
.IT
stockade [`s'] -- Stockades can hold various resources, as well as
increase the defense of troops garrisoned within.  They are, however,
not among the most congenial of accommodations, and are usually only
placed in sectors which cannot adequately support civilians.
.IT
towns [`t'] -- A town sector makes a good living location for both
civilians and soldiers.  While not quite possessing the size or
capabilities of a full city, it does provide for its own defenses.
.IT
cities [`c'] -- City sectors can become the backbone of your nation,
distributing resources to many neighboring sectors.  Defensive
potential is excellent.
.IT
capital [`C'] -- A capital is the headquarters of a nation's
government.  A nation without a designated capital can become
unfocused and may be susceptible to unrest.
.UT
.PP
Economic stability depends upon good selection of designations.
.RF "Economics" "Economic Management"
.CE
.CS "The Cost of Designations"
.PP
In order to actually construct the designations for a sector, each
nation must spend a certain amount of raw materials depending on which
designation is being built and what constructions are already within
the sector.  It is important to note that the raw materials must be
available in a nearby supply center.  Otherwise, the materials must be
carried into the sector via ships or wagons.
.PP
The base expenditures needed for each designation type is as
follows:
.TB
         Sector Designation           wood       metals      talons
         ----------------------     --------    --------    --------
         None                            0           0         500
         Farm                            0           0        1000
         Metal Mine                    100         200        3000
         Jewel Mine                    100         200        3000
         Lumberyard                     50         200        2000
         Shrine +                      150         500        5000
         Bridge                      15000        3000       50000
         Canal                        7500        4000       40000
         Wall                        10000        5000       35000
         Cache *                      1000        2000        2000
         Stockade *                  10000        1000       10000
         Town *                      10000       10000       10000
         City **                      5000       50000       20000
         Capital *                    1000        5000      150000

            *  - this designation takes one month to construct
            ** - this designation takes three months to construct
            +  - 1000 jewels are also needed to build a shrine
.sp
          Sectors may not be designated as Fertile or Fruitful, so
            there is no listing for either of these sector types.
.TE
.PP
It might be noted that with the construction of a city, most of the
construction costs deal with the building up of the fortifications to
more solid metal constructions, and the added expense of increased
city government.  With the Capital, almost all of the expenses are for
the relocation of the national government.
.CE
.CS "Designation Support Costs"
.PP
It would be nice if sectors, once built did not need any upkeep in
order to survive, but in life things are never so easy.  So, each
turn, some raw materials will have to be allocated towards upkeep.
The amount of the needed upkeep is greatly affected by the type of
designation within the sector.
.PP
The chart below indicates the cost per turn for each designation type
in metals, wood, and talons.  Also listed is the construction
multiplier, which indicates how many times the normal price a minor
designation (see the next section) will cost for that particular
designation.
.TB
        Sector Designation       wood      metals     talons     Mult
      ----------------------   --------   --------   --------   ------
       None                         0          0          0        1
       Farm                         0          0         20        1
       Fertile                      0          0         20        1
       Fruitful                     0          0         20        1
       Metal Mine                   5         10         60        1
       Jewel Mine                   5         10         60        1
       Lumberyard                   5          8         30        1
       Shrine +                    10         40         50        1
       Bridge                      50         50       1000        3
       Canal                      200         60       1500        2
       Wall                       100        100       1000        2
       Cache                        0          0          0        1
       Stockade                   200        100       3000        3
       Town                       200         20       1000        5
       City                       200         50       3000        8
       Capital                    220         60       3500        8
.sp
         +   = 10 jewels are also needed to support shrines
.TE
.PP
In addition to all of the above support costs, there is also the
matter of food.  For every person in a sector there must be at
approximately 1 food unit per person per turn, or starvation will
begin.  Of course, as the food supply varies, the population will tend
to consume more or less than just 1 food unit per month each, as
indicated by the "eat rate" of the nation.
.CE
.CS "Minor Designations"
.PP
As with the real world, there are often many traits to an area of land
which can be common even among land used for the most diverse of
purposes.  Thus, I have made it possible for sectors to have traits
which are a property of the sector, but not a designation.  Since many
of these traits actually were full fledged designations in prior
versions of conquer, I call them "minor designations", or
constructions.
.PP
Most of the minor designations are able to be built by the nation
itself using the command "construct".  But, some, such as "devastated",
"sieged", and "for sale" are the results of battles taking place, or
other events causing the trait to be present within that sector.  The
full list and description of the minor designations are:
.IT
devastated -- Damage to the land and buildings on the sector has
caused it to be greatly reduced in production ability and use.  A
sector with such a trait has only a quarter of the productivity of a 
normal sector of the same class.  The civilians also find it extremely
less attractive to live in.
.IT
for sale -- If the sector has been placed on the trading block, it is
indicated by the possession of this trait.
.IT
sieged -- When enemy armies have encamped their armies around the town
or fortress within the sector, and they have successfully implemented
their blockade, this trait will be visible.  Troops are stuck within
the fortifications, civilians and materials will not be allowed to
move either into or out of the sector, and if the sector is a water
sector, enemy ships will be unable to enter the sector.
.IT
trading post -- A site for the exchanging of materials, a trading post
is also conducive to business and productivity within the sector.
.IT
roads -- A sector with roads within it is much easier for military
units to move through.  As a matter of fact, the movement is twice as
easy with roads as without.
.IT
blacksmith -- In a society geared around the use of metals, a town
blacksmith can prove of great benefit.  When a sector possesses such
an able body, metal production can be increased and actual consumption
decreased.
.IT
university -- The scholarly pursuits often lead to the enhancement of
the knowledge in a nation.
.IT
church -- The backbone of many cultures, religion serves to increase
the productivity of the masses.
.IT
mill -- A site for processing grain, a mill will increase the
productivity of farms and extend the usefulness of the grain itself.
.IT
granary -- Used to store grain to reduce spoilage and to provide food
for the winter months.
.IT
fortified -- As much of the world revolves around the use of force,
protecting territory is often necessary for survival.  Even the
scattered farms can have some form of rudimentary protection in place
to protect soldiers and citizens alike.
.IT
harbor -- With naval power being of great interest to many countries,
having a harbor in a town or city will allow fleets easier landing, as
well as providing less of a hassle when loading and unloading goods.
.UT
.CE
.CS "The Costs of Construction"
.PP
While it would be nice if things were free in this world, there is
usually a price for everything.  And, the minor designations are no
exception to the rule.  The chart below shows the expenses to build
each minor designation.  Note that this number is multiplied by the
"multiplier" of the major designation of the sector.  The added
expense for the towns and cities is due to the size of such
constructions within the larger towns and cities.  [Where a farm might
have only 1 or 2 blacksmiths, a city would have many more.]
.TB
         Sector Designation           wood       metals      talons
        ----------------------      --------    --------    --------
         devastated                      0           0        1000
         for sale                        0           0           0
         sieged                          0           0           0
         trading post                  200           0        5000
         roads                        1000          20        3000
         blacksmith                      0        1000        2500
         university                    300           0        2500
         church                        200          10        3000
         mill                          300         500        3000
         granary                       100          50        2000
         fortified                    1000        2000        5000
         harbor                       1000         500        3000
.TE
.PP
And, of course, there are support expenses to worry about.  Many of
these expenses are just administrative costs, but most are to assure
the upkeep of the facility.  These expenses are also affected by the
"multiplier".  The list of minor designation support costs is:
.TB
         Sector Designation           wood       metals      talons
        ----------------------      --------    --------    --------
         devastated                      0           0           0
         for sale                        0           0           0
         sieged                          0           0         200
         trading post                    1           1        1000
         roads                          20           1         400
         blacksmith                      0         200         400
         university                     10           0         500
         church                         10           0         300
         mill                           50          25         200
         granary                        10           3         200
         fortified                     100         100         500
         harbor                        100          25         100
.TE
.CE
.CS "The Use of Minor Designations"
.PP
The minor designations are generally of great benefit to a sector, and
the impact which the construction has is usually dependent on the
major designation of the sector.   To be most explicit, if the sector
is a supply center, the minor designations of the sector will affect
materials as they are stored in, or distributed from the supply
center.  If the sector is not a supply center, then the designation
will affect materials as they are produced.  This distinction can
prove crucial in helping to decide where and when to make
constructions upon a sector.
.PP
The two charts below give a description of what the minor designations
are useful for in each case:
.TB
  Construction          Use within Non-Supply Centers
 ----------------      -------------------------------------------------
  devastated            productivity at 1/4, attractiveness at 1/8 normal
  for sale              no affect, a marker for a sale
  sieged                troops trapped, production down 80%, taxation down 20%
  trading post          productivity and taxability up by 5%
  roads                 sector movement cost reduced by half
  blacksmith            increase metal production by 10%
  university            production and talon income up by 3%
  church                talon income down by 5%, production up by 10%
  mill                  food production up by 10%
  granary               taxability up by 3%, food storage provides for winters
  fortified             provide a 10% combat bonus to garrisoned troops
  harbor                harbors should not be possible in non-supply centers
.TE
.TB
  Construction          Use within Supply Centers
 ----------------      -------------------------------------------------
  devastated            productivity at 1/4, attractiveness at 1/8 normal
  for sale              a supply center should not be able to be sold
  sieged                troops trapped, no supplies go in or out
  trading post          provide a location for exchanging with other nations
  roads                 decrease movement cost by 50% within sector
  blacksmith            decrease regional metal expenditures by 10%
  university            provide a site for the increase of knowledge
  church                provide a method of increasing population happiness
  mill                  decrease regional food expenditures by 10%
  granary               decrease spoil rate of food within the supply center
  fortified             provide defensive cover for garrisoned troops
  harbor                provide site for construction and docking of navies
.TE
.CE
