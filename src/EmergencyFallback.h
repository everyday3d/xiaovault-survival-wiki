#pragma once
#include <Arduino.h>

// =============================================================================
// EMERGENCY SURVIVAL REFERENCE (STORED IN FLASH PROGMEM)
// Accessible even if MicroSD card is removed, unformatted, or corrupted!
// =============================================================================

struct EmergencyGuide {
    const char* id;
    const char* title;
    const char* category;
    const char* icon;
    const char* contentHtml;
};

// 1. First Aid & Adult CPR
const char EMERGENCY_CPR_HTML[] PROGMEM = R"rawhtml(
<h3>🚨 Emergency Adult CPR (Cardiopulmonary Resuscitation)</h3>
<p><strong>Step 1: Check Responsiveness</strong><br>
Tap shoulders firmly and shout: <em>"Are you OK?"</em>. Check for breathing (no more than 10 seconds).</p>
<p><strong>Step 2: Call for Help</strong><br>
Direct a specific bystander: <em>"You, call emergency services and get an AED!"</em>.</p>
<p><strong>Step 3: Hand Placement</strong><br>
Place heel of one hand in center of chest (lower half of breastbone). Interlock fingers of second hand on top. Keep arms straight, shoulders directly over hands.</p>
<p><strong>Step 4: Chest Compressions (100–120 BPM)</strong><br>
Push hard and fast: <strong>2 inches (5 cm) deep</strong>. Allow chest to fully recoil between compressions.<br>
<em>Rhythm: Think of the beat to "Stayin' Alive" by the Bee Gees.</em></p>
<p><strong>Step 5: Compression-to-Breath Ratio</strong><br>
Perform <strong>30 chest compressions</strong>, followed by <strong>2 rescue breaths</strong> (tilt head back, pinch nose, blow until chest visibly rises). If untrained, perform continuous hands-only CPR.</p>
)rawhtml";

// 2. Severe Bleeding & Tourniquet Application
const char EMERGENCY_BLEEDING_HTML[] PROGMEM = R"rawhtml(
<h3>🩸 Severe Bleeding & Tourniquet Application</h3>
<p><strong>Step 1: Direct Pressure</strong><br>
Apply firm, continuous pressure directly over the wound with sterile gauze or clean cloth. Do NOT remove soaked gauze; add more on top.</p>
<p><strong>Step 2: Wound Packing</strong><br>
For deep junctional wounds (groin, armpit, neck), tightly pack sterile gauze deep into the wound cavity until full, then hold intense pressure for 3+ minutes.</p>
<p><strong>Step 3: Extremity Tourniquet (Arms & Legs)</strong><br>
If bleeding is arterial (bright red, spurting) or uncontrollable:
<ol>
  <li>Place tourniquet <strong>2–3 inches above wound</strong> (never directly over a joint).</li>
  <li>Pull band as tight as possible before turning windlass.</li>
  <li>Twist windlass rod until bright red bleeding completely stops.</li>
  <li>Lock windlass in clip.</li>
  <li><strong>Write exact time of application (e.g. "T 14:30") on patient's forehead or tape.</strong></li>
  <li>NEVER loosen or remove a tourniquet in the field once applied.</li>
</ol></p>
)rawhtml";

// 3. Emergency Water Purification
const char EMERGENCY_WATER_HTML[] PROGMEM = R"rawhtml(
<h3>💧 Emergency Water Purification</h3>
<p>Never drink untreated surface water (Giardia, Cryptosporidium, bacteria, and viruses cause fatal dehydration).</p>
<h4>Method 1: Boiling (Gold Standard)</h4>
<ul>
  <li>Bring water to a <strong>rolling boil for at least 1 full minute</strong> (3 minutes at elevations above 6,500 ft / 2,000 m).</li>
  <li>Kills 100% of pathogens (protozoa, bacteria, and viruses).</li>
</ul>
<h4>Method 2: Household Bleach (Plain Unscented 5–6% Sodium Hypochlorite)</h4>
<ul>
  <li>Clear water: Add <strong>2 drops per quart / liter</strong> (8 drops per gallon).</li>
  <li>Cloudy water: Add <strong>4 drops per quart / liter</strong> (16 drops per gallon).</li>
  <li>Stir and let stand for <strong>30 minutes</strong>. Water should have a slight chlorine scent.</li>
</ul>
<h4>Method 3: Improvised Charcoal & Sand Biofilter (Pre-Filtration)</h4>
<p>Cut bottom off a 2-liter bottle. Layer from bottom to top:
<ol>
  <li>Clean cloth / coffee filter at nozzle.</li>
  <li>Finely crushed hardwood charcoal (removes toxins & odor).</li>
  <li>Fine clean sand.</li>
  <li>Coarse sand / fine gravel.</li>
  <li>Small pebbles (catches large sediment).</li>
</ol>
<em>Note: Sand/charcoal filter clears sediment and heavy toxins but does NOT kill viruses/bacteria. Always boil filtered water afterwards!</em></p>
)rawhtml";

// 4. Survival Rule of Threes & Shelter
const char EMERGENCY_RULE_OF_THREES_HTML[] PROGMEM = R"rawhtml(
<h3>🏕️ The Survival Rule of Threes</h3>
<ul>
  <li><strong>3 Minutes without Oxygen</strong> (Airway blockage, drowning, smoke inhalation).</li>
  <li><strong>3 Hours without Shelter</strong> (Hypothermia in cold/wet or heat stroke in desert).</li>
  <li><strong>3 Days without Water</strong> (Dehydration, organ failure).</li>
  <li><strong>3 Weeks without Food</strong> (Caloric depletion, lethargy).</li>
</ul>
<h4>Debris Hut Shelter (Emergency Insulation)</h4>
<ol>
  <li>Find a sturdy ridge pole ~9–10 ft long; prop one end on a stump or rock ~3 ft high.</li>
  <li>Lean ribs (sticks) at 45° angles along both sides, leaving a small crawl space entry.</li>
  <li>Cover ribs with smaller lattice twigs.</li>
  <li>Pyle dry leaves, pine needles, moss, or dry grass at least <strong>2 to 3 feet thick</strong> over the entire shelter.</li>
  <li>Fill interior floor with 1 foot of dry insulation (the ground drains body heat faster than air!).</li>
</ol>
)rawhtml";

// 5. Morse Code & International Distress Signals
const char EMERGENCY_SIGNALS_HTML[] PROGMEM = R"rawhtml(
<h3>📡 Emergency Distress Signals & Morse Code</h3>
<h4>Universal Distress: The Rule of 3</h4>
<p>Any signal repeated <strong>3 times in a row</strong> indicates life-threatening emergency:
<ul>
  <li><strong>3 loud whistle blasts</strong> (pause 1 minute, repeat).</li>
  <li><strong>3 gunshots</strong> spaced 5 seconds apart.</li>
  <li><strong>3 fires in a triangle or straight line</strong> (international aviation distress signal).</li>
  <li><strong>3 mirror flashes</strong> aimed at aircraft or horizon.</li>
</ul></p>
<h4>SOS in Morse Code: <code>··· — — — ···</code></h4>
<p>3 Short, 3 Long, 3 Short. Repeat every 10 seconds.</p>
<pre>
A: ·—       B: —···     C: —·—·     D: —··      E: ·
F: ··—·     G: ——·      H: ····     I: ··       J: ·———
K: —·—      L: ·—··     M: ——       N: —·       O: ———
P: ·——·     Q: ——·—     R: ·—·      S: ···      T: —
U: ··—      V: ···—     W: ·——      X: —··—     Y: —·——
Z: ——··     1: ·————    2: ··———    3: ···——    4: ····—
5: ·····    6: —····    7: ——···    8: ———··    9: ————·
0: —————
</pre>
)rawhtml";

// Array of built-in emergency flash guides
const EmergencyGuide EMERGENCY_GUIDES[] = {
    {"cpr", "Adult CPR & Airway Management", "First Aid", "❤️", EMERGENCY_CPR_HTML},
    {"bleeding", "Severe Bleeding & Tourniquet", "First Aid", "🩸", EMERGENCY_BLEEDING_HTML},
    {"water", "Water Purification & Filtration", "Hydration", "💧", EMERGENCY_WATER_HTML},
    {"shelter", "Rule of Threes & Debris Shelter", "Shelter", "🏕️", EMERGENCY_RULE_OF_THREES_HTML},
    {"signals", "Distress Signals & Morse Code", "Signaling", "📡", EMERGENCY_SIGNALS_HTML}
};

const size_t EMERGENCY_GUIDES_COUNT = sizeof(EMERGENCY_GUIDES) / sizeof(EmergencyGuide);
