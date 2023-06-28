import { Ghost } from "./Ghost"

export class SignalGhost extends Ghost
{
	constructor()
	{
		this.color = "#FF8888";
	}

	special(map, pacman, ghosts, mapSize)
	{
		this.firstTarget = false;

		if (this.lineOfSight(map, pacman, ghosts, mapSize) && !pacman.getPower())
		{
			this.hasLineOfSight = true;
			for (let ghost in ghosts)
				ghost.setTimer(5 * 60);
		}
		else
			this.hasLineOfSight = false;

		if (!pacman.getPower())
		{
			this.hasTarget = true;
			this.target = pacman.getPos();
		}
	}

	getType()
	{
		return 1;
	}
}