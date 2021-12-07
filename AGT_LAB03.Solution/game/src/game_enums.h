#pragma once
#ifndef GAME_ENUMS
#define GAME_ENUMS

namespace game_enums {
	enum class state {
		idle,
		patrol,
		approach,
		shoot,
		turnOffSwitch,
		placingTurret,
		placingOffensiveTurret,
		flee,
		melee,

		stuck,
		attemptingUnstick,
		decision
	};

	enum class enemies {
		ranged,
		rangedShotgun,
		engineer,
		melee
	};
}
#endif
