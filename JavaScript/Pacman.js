import { Vector2 } from "./lib/vector2"

export class Pacman
{

    constructor()
    {
        this.pos = new Vector2(), this.prog = new Vector2();
        this.speed = 7, this.dir = 3, this.damageTimer = 0;
        this.power = false, this.win = false;
    }

    start(pos)
    {
        this.pos = pos;
        prog.x = 50;
    }

    move(dir, map, dots, mapSize)
    {

        for (let i = 0; i < this.speed * (1 + (this.power || !dots)); i++)
        {
            if (!this.prog.x && !this.prog.y)
            {
                switch (dir)
                {
                    case 0:
                        if (this.canMove(this.pos.x, this.pos.y - 1, map, dots, mapSize))
                            this.dir = dir;
                        break;
                    case 1:
                        if (this.canMove(this.pos.x + 1, this.pos.y, map, dots, mapSize))
                            this.dir = dir;
                        break;
                    case 2:
                        if (this.canMove(this.pos.x, this.pos.y + 1, map, dots, mapSize))
                            this.dir = dir;
                        break;
                    case 3:
                        if (this.canMove(this.pos.x - 1, this.pos.y, map, dots, mapSize))
                            this.dir = dir;
                }
            }

            switch (this.dir)
            {
                case 0:
                    if (this.canMove(this.pos.x, this.pos.y - 1, map, dots, mapSize))
                        this.prog.y--;
                    break;
                case 1:
                    if (this.canMove(this.pos.x + 1, this.pos.y, map, dots, mapSize))
                        this.prog.x++;
                    break;
                case 2:
                    if (this.canMove(this.pos.x, this.pos.y + 1, map, dots, mapSize))
                        this.prog.y++;
                    break;
                case 3:
                    if (this.canMove(this.pos.x - 1, this.pos.y, map, dots, mapSize))
                        this.prog.x--;
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
            {
                if (!dots)
                    this.win = true;
                this.pos.x = mapSize.x - 1;
            }
            if (this.pos.x == mapSize.x)
            {
                if (!dots)
                    this.win = true;
                this.pos.x = 0;
            }
            if (this.pos.y == -1)
            {
                if (!dots)
                    this.win = true;
                this.pos.y = mapSize.y - 1;
            }
            if (this.pos.y == mapSize.y)
            {
                if (!dots)
                    this.win = true;
                this.pos.y = 0;
            }
        }

        if (this.damageTimer)
        {
            this.damageTimer--;
            if (!this.damageTimer)
                this.power = false;
        }
    }

    getPos()
    {
        return this.pos;
    }

    getProg()
    {
        return this.prog;
    }

    getDir()
    {
        return this.dir;
    }

    damage()
    {
        if (!this.damageTimer)
            this.damageTimer = 3 * 60;
    }

    getDamageTimer()
    {
        return this.damageTimer;
    }

    setPower()
    {
        this.damageTimer = 5 * 60;
        this.power = true;
    }

    getPower()
    {
        return this.power;
    }

    hasWon()
    {
        return this.win;
    }

    setDir(dir)
    {
        if (dir == 1 || dir == 3)
            this.dir = dir;
    }

    canMove(x, y, map, dots, mapSize)
    {
        if (x >= 0 && y >= 0 && y < map.size() && x < map[0].size())
            return (map[y][x] == 0 || map[y][x] == 7 || map[y][x] == 8 || map[y][x] == 9);
        if (x == -1 || x == mapSize.x || y == -1 || y == mapSize.y)
            return true;
        return false;
    }
}