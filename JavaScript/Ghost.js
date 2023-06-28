import { Pacman } from "./Pacman"

export class Ghost
{

    constructor()
    {
        this.color = "#FF00FF";

        this.pos = {x: 0, y: 0}, this.prog = {x: 0, y: 0}, this.target = {x: 0, y: 0}, this.spawn = {x: 0, y: 0};
        this.speed = 3, this.defSpeed = this.speed, this.dir = 3, this.health = 1, this.timer = 0, this.slowTimer = 0;
        this.hasTarget = false, this.hasLineOfSight = false, this.free = false, this.firstTarget = true;
    }

    start(pos)
    {
        this.pos = pos;
        this.spawn = pos;
        this.prog.x = 50;
        this.prog.y = 0;
        this.dir = Math.floor(Math.random() * 2) * 2 + 1;
        this.hasTarget = false;
        this.timer = 0;
        this.speed = 3;
        this.free = false;
    }

    move(map, pacman, ghosts, mapSize)
    {
        let minDist, dir, value;
        let taken;

        for (let i = 0; i < this.speed / ((this.slowTimer != 0) + 1) * (1 + this.firstTarget * 0.5); i++)
        {
            minDist = 999;
            dir = -1;

            if (this.free)
            {

                if (!this.prog.x && !this.prog.y)
                {
                    if (this.dir != 2 && this.canMove(this.pos.x, this.pos.y - 1, map, mapSize))
                    {
                        taken = false;
                        for (let ghost in ghosts)
                            if (this.pos.x == ghost.getPos().x && this.pos.y - 1 == ghost.getPos().y)
                                taken = true;
                        value = Math.sqrt(Math.pow(this.pos.x - this.target.x, 2) + Math.pow(this.pos.y - 1 - this.target.y, 2));
                        if (value < minDist && !taken)
                        {
                            dir = 0;
                            minDist = value;
                        }
                    }
                    if (this.dir != 3 && this.canMove(this.pos.x + 1, this.pos.y, map, mapSize))
                    {
                        taken = false;
                        for (let ghost in ghosts)
                            if (this.pos.x + 1 == ghost.getPos().x && this.pos.y == ghost.getPos().y)
                                taken = true;
                        value = Math.sqrt(Math.pow(this.pos.x + 1 - this.target.x, 2) + Math.pow(this.pos.y - this.target.y, 2));
                        if (value < minDist && !taken)
                        {
                            dir = 1;
                            minDist = value;
                        }
                    }
                    if (this.dir != 0 && this.canMove(this.pos.x, this.pos.y + 1, map, mapSize))
                    {
                        taken = false;
                        for (let ghost in ghosts)
                            if (this.pos.x == ghost.getPos().x && this.pos.y + 1 == ghost.getPos().y)
                                taken = true;
                        value = Math.sqrt(Math.pow(this.pos.x - this.target.x, 2) + Math.pow(this.pos.y + 1 - this.target.y, 2));
                        if (value < minDist && !taken)
                        {
                            dir = 2;
                            minDist = value;
                        }
                    }
                    if (this.dir != 1 && this.canMove(this.pos.x - 1, this.pos.y, map, mapSize))
                    {
                        taken = false;
                        for (let ghost in ghosts)
                            if (this.pos.x - 1 == ghost.getPos().x && this.pos.y == ghost.getPos().y)
                                taken = true;
                        value = Math.sqrt(Math.pow(this.pos.x - 1 - this.target.x, 2) + Math.pow(this.pos.y - this.target.y, 2));
                        if (value < minDist && !taken)
                        {
                            dir = 3;
                            minDist = value;
                        }
                    }
                    if (dir != -1 && this.dir != dir)
                        this.dir = dir;
                }

                switch (this.dir)
                {
                    case 0:
                        if (this.canMove(this.pos.x, this.pos.y - 1, map, mapSize))
                            this.prog.y--;
                        break;
                    case 1:
                        if (this.canMove(this.pos.x + 1, this.pos.y, map, mapSize))
                            this.prog.x++;
                        break;
                    case 2:
                        if (this.canMove(this.pos.x, this.pos.y + 1, map, mapSize))
                            this.prog.y++;
                        break;
                    case 3:
                        if (this.canMove(this.pos.x - 1, this.pos.y, map, mapSize))
                            this.prog.x--;
                }

                this.special(map, pacman, ghosts, mapSize);
            }
            else
            {
                if (this.canMove(this.pos.x, this.pos.y, map, mapSize))
                    this.free = true;
                else
                    this.prog.y--;
            }

            if (this.prog.x >= 100)
            {
                this.pos.x++;
                this.prog.x -= 100;
            }
            if (this.prog.x <= -100)
            {
                this.pos.x--;
                this.prog.x += 100;
            }
            if (this.prog.y >= 100)
            {
                this.pos.y++;
                this.prog.y -= 100;
            }
            if (this.prog.y <= -100)
            {
                this.pos.y--;
                this.prog.y += 100;
            }

            if (this.pos.x == -1)
                this.pos.x = mapSize.x; - 1
            if (this.pos.x == mapSize.x)
                this.pos.x = 0;
        }

        if (this.timer)
            this.timer--;
        if (this.slowTimer)
            this.slowTimer--;
    }

    getPos()
    {
        return this.pos;
    }

    getProg()
    {
        return this.prog;
    }

    getTarget()
    {
        return this.target;
    }

    getDir()
    {
        return this.dir;
    }

    lineOfSight(map, pacman, ghosts, mapSize)
    {
        if (this.pos.x == pacman.getPos().x && this.pos.y == pacman.getPos().y)
        {
            if (!pacman.getDamageTimer())
            {
                this.start(this.spawn);
                if (!pacman.getDamageTimer())
                {
                    pacman.damage();
                    lives--;
                }
                for (let ghost in ghosts)
                    ghost.slow();
            }
            return true;
        }
        else if (this.pos.x == pacman.getPos().x)
        {
            if (this.pos.y < pacman.getPos().y)
            {
                for (let i = this.pos.y; i <= pacman.getPos().y; i++)
                    if (!this.canMove(this.pos.x, i, map, mapSize))
                        return false;
                return true;
            }
            else if (this.pos.y > pacman.getPos().y)
            {
                for (let i = pacman.getPos().y; i <= this.pos.y; i++)
                    if (!this.canMove(this.pos.x, i, map, mapSize))
                        return false;
                return true;
            }
        }
        else if (this.pos.y == pacman.getPos().y)
        {
            if (this.pos.x < pacman.getPos().x)
            {
                for (let i = this.pos.x; i <= pacman.getPos().x; i++)
                    if (!this.canMove(i, this.pos.y, map, mapSize))
                        return false;
                return true;
            }
            else if (this.pos.x > pacman.getPos().x)
            {
                for (let i = pacman.getPos().x; i <= this.pos.x; i++)
                    if (!this.canMove(i, this.pos.y, map, mapSize))
                        return false;
                return true;
            }
        }
        return false;
    }

    special(map, pacman, ghosts, mapSize)
    {

    }

    updateTarget(map, pacman, mapSize)
    {
        if (this.target == this.pos)
        {
            this.firstTarget = false;
            this.hasTarget = false;
        }
        if (!this.hasTarget)
        {
            this.target.x = Math.floor(Math.random() * mapSize.x);
            this.target.y = Math.floor(Math.random() * mapSize.y);

            if (canMove(this.target.x, this.target.y, map, mapSize))
                this.hasTarget = true;
            else
                updateTarget(map, pacman, mapSize);
        }
    }

    setTimer(value)
    {
        if (this.timer < value)
            this.timer = value;
    }

    getColor()
    {
        return this.color;
    }

    getType()
    {
        return -1;
    }

    getTimer()
    {
        return this.timer;
    }

    slow()
    {
        this.slowTimer = 5 * 60;
    }

    hasLOS()
    {
        return this.hasLineOfSight;
    }

    loseTarget()
    {
        this.hasTarget = false;
    }

    canMove(x, y, map, mapSize)
    {
        if (x >= 0 && y >= 0 && y < map.size() && x < map[0].size())
            return (map[y][x] == 0 || map[y][x] == 7 || map[y][x] == 8 || map[y][x] == 9);
        if (x == -1 || x == mapSize.x || y == -1 || y == mapSize.y)
            return true;
        return false;
    }
}