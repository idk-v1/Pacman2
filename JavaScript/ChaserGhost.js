import { Ghost } from "./Ghost"

export class ChaserGhost extends Ghost
{
	constructor()
	{
		this.color = "#8888FF";
	}

	special(map, pacman, ghosts, mapSize)
	{
		if (this.lineOfSight(map, pacman, ghosts, mapSize) && !pacman.getPower())
		{
			this.setTimer(2 * 60);
			this.hasLineOfSight = true;
		}
		else
			this.hasLineOfSight = false;

		if (this.timer && !this.firstTarget)
		{
			this.hasTarget = true;
			this.target = pacman.getPos();
			this.speed = this.defSpeed + 2 + this.hasLineOfSight * 3;
		}
		else
			this.speed = this.defSpeed;
	}

	getType()
	{
		return 0;
	}
}