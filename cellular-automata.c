//gcc cellular-automata.c -o cellular-automata ; ./cellular-automata ; type cellular-automata-output.obj
//https://softologyblog.wordpress.com/2019/12/28/3d-cellular-automata-3/
//https://stackoverflow.com/questions/822323/how-to-generate-a-random-int-in-c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef char Cell;

const char rule_alive [] =
{
    0, // 0
    0, // 1
    0, // 2
    0, // 3
    0, // 4
    0, // 5
    0, // 6
    0, // 7
    0, // 8
    1, // 9
    1, // 10
    1, // 11
    1, // 12
    1, // 13
    1, // 14
    1, // 15
    1, // 16
    1, // 17
    1, // 18
    1, // 19
    1, // 20
    1, // 21
    1, // 22
    1, // 23
    1, // 24
    1, // 25
    1, // 26
};
const char rule_born [] =
{
    0, // 0
    0, // 1
    0, // 2
    0, // 3
    0, // 4
    1, // 5
    1, // 6
    1, // 7
    0, // 8
    0, // 9
    0, // 10
    0, // 11
    1, // 12
    1, // 13
    0, // 14
    1, // 15
    0, // 16
    0, // 17
    0, // 18
    0, // 19
    0, // 20
    0, // 21
    0, // 22
    0, // 23
    0, // 24
    0, // 25
    0, // 26
};
const int rule_states = 5;

typedef struct {
    int x;
    int y;
    int z;
} V;

const int cube_v[] = {
    0, 0, 0,
    0, 0, 1,
    0, 1, 0,
    0, 1, 1,
    1, 0, 0,
    1, 0, 1,
    1, 1, 0,
    1, 1, 1,
};

const int cube_c[] = {
    -1, -1, -1,
    -1, -1, 0,
    -1, -1, 1,
    -1, 0, -1,
    -1, 0, 0,
    -1, 0, 1,
    -1, 1, -1,
    -1, 1, 0,
    -1, 1, 1,
    0, -1, -1,
    0, -1, 0,
    0, -1, 1,
    0, 0, -1,
    //0, 0, 0,
    0, 0, 1,
    0, 1, -1,
    0, 1, 0,
    0, 1, 1,
    1, -1, -1,
    1, -1, 0,
    1, -1, 1,
    1, 0, -1,
    1, 0, 0,
    1, 0, 1,
    1, 1, -1,
    1, 1, 0,
    1, 1, 1,
};

const int cube_f[] = {
    6, 8, 7, 5,
    3, 4, 8, 7,
    1, 2, 4, 3,
    1, 2, 6, 5,
    2, 4, 8, 6,
    1, 3, 7, 5,
};

V v (int x, int y, int z)
{
    V v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

V vadd (V v1, V v2)
{
    V v;
    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;
    v.z = v1.z + v2.z;
    return v;
}

int getCell(V pos, Cell *data, V size)
{
   return (int)data[pos.z*size.x*size.y + pos.x*size.y + pos.y];
}

void setCell(int state, V pos, Cell *data, V size)
{
    data[pos.z*size.x*size.y + pos.x*size.y + pos.y] = state;
}

FILE *fp;
int offset = 0;
int iteration = 0;

static void draw_cube (V pos)
{
    for (int i = 0; i < 8; i++)
    {
       int j = i * 3;
       V vec = v(cube_v[j], cube_v[j+1], cube_v[j+2]);
       V voffset = v(pos.x, pos.y, pos.z);
       vec = vadd(vec, voffset);
       fprintf(fp, "v %d %d %d\n", vec.x, vec.y, vec.z);
    }
    for (int i = 0; i < 6; i++)
    {
       int j = i * 4;
       fprintf(fp, "f %d %d %d %d\n", cube_f[j] + offset, cube_f[j+1] + offset, cube_f[j+2] + offset, cube_f[j+3] + offset);
    }
    offset += 8;
}

static void draw_pass (int pass, Cell *data, V size)
{
    for (int x = 1; x < size.x - 1; x++)
    {
       for (int y = 1; y < size.y - 1; y++)
       {
           for (int z = 1; z < size.z - 1; z++)
           {
               if (getCell(v(x,y,z), data, size) == pass)
               {
                   char draw = 6;

                   // optimise
                   if (getCell(v(x,y,z+1), data, size) > 0)
                       draw--;
                   if (getCell(v(x,y,z-1), data, size) > 0)
                       draw--;
                   if (getCell(v(x,y+1,z), data, size) > 0)
                       draw--;
                   if (getCell(v(x,y-1,z), data, size) > 0)
                       draw--;
                   if (getCell(v(x+1,y,z), data, size) > 0)
                       draw--;
                   if (getCell(v(x-1,y,z), data, size) > 0)
                       draw--;

                   if (draw > 0)
                       draw_cube(v(x,y,z));
               }
           }
       }
    }
    printf("drew %d verticies\n", offset);
}

int main (int argc, char *argv[])
{

    printf("initialising...\n");
    V size = v(100, 100, 100);
    Cell *data = (Cell*)malloc((Cell)size.x*size.y*size.x);
    Cell *outdata = (Cell*)malloc((Cell)size.x*size.y*size.x);
    for (int x = 50; x < 60; x++)
    {
       for (int y = 50; y < 60; y++)
       {
           for (int z = 50; z < 60; z++)
           {
               V pos = v(x,y,z);
               setCell(((int)rand() % 2) * rule_states, pos, data, size);
           }
       }
    }
    for (int iter = 0; iter < 100; iter++)
    {
        iteration++;
        printf("===ITERATION %d===\n", iteration);
        printf("calculating...\n");
        srand(time(NULL));   // Initialization, should only be called once.
        for (int x = 1; x < size.x - 1; x++)
        {
           for (int y = 1; y < size.y - 1; y++)
           {
               for (int z = 1; z < size.z - 1; z++)
               {
                   V pos = v(x,y,z);
                   int neighbours = 0;
                   for (int i = 0; i < 26; i++)
                   {
                       int j = i * 3;
                       V vec = v(cube_c[j], cube_c[j+1], cube_c[j+2]);
                       vec = vadd(vec, pos);
                       if (getCell(vec, data, size) > rule_states - 2)
                           neighbours++;
                   }
                   if (rule_born[neighbours] == 1 && getCell(pos, data, size) == 0)
                   {
                       setCell(rule_states - 1, pos, outdata, size);
                   }
                   if (rule_alive[neighbours] == 1 && getCell(pos, data, size) >  rule_states - 2)
                   {
                       setCell(rule_states - 1, pos, outdata, size);
                   }
                   else if (getCell(pos, data, size) > 0)
                       setCell((int)getCell(pos, data, size) - 1, pos, outdata, size);
               }
           }
        }

        for (int x = 1; x < size.x - 1; x++)
        {
           for (int y = 1; y < size.y - 1; y++)
           {
               for (int z = 1; z < size.z - 1; z++)
               {
                   V pos = v(x,y,z);
                   setCell(getCell(pos, outdata, size), pos, data, size);
                   setCell(0, pos, outdata, size);
               }
           }
        }
        printf("drawing...\n");

        for (int pass = 1; pass < rule_states; pass++)
        {
            const char *filenameprefix = "out/ca-p";
            const char *filenamesuffix = ".obj";

            char filename[32];
            char snum[5];
            strcpy(filename, filenameprefix);
            itoa(pass, snum, 10);
            strcat(filename, snum); 
            strcat(filename, "-i");
            itoa(iteration, snum, 10);
            strcat(filename, snum); 
            strcat(filename, filenamesuffix);
            fp = fopen(filename, "w+");
            printf("writing to %s\n", filename);
            fprintf(fp, "o ca-p%d-i\n", pass);
            draw_pass(pass, data, size);
            offset = 0;
        }
    }
    fclose(fp);
}
