#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;  //aniation frame
    float updateTime; //amount of time before we update the animation frame
    float runningTime;
};

// function for carfy is on the ground or not (its return true or false)
bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

//Return AnimDtata
AnimData updateAnimData( AnimData data, float deltaTime, int maxFrame)
{
    //update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        //update animation frame 
        data.rec.x = data.frame * data.rec.width;
        data.frame ++;
        if (data.frame > maxFrame)
        {
            data.frame =0;
        }
    }
    return data;
}



int main()
{
    // array with window dimentions
    int windowDimensions[2];
    windowDimensions[0] = 512;
    windowDimensions[1] = 380;

    // initialize the window
    InitWindow(windowDimensions[0],windowDimensions[1], "Dapper Dasher Run Game!");
 
    // acceleration due to gravity (pixels/s)/s
    const int gravity{1'000};
    
    // nebula variables
    Texture2D nebula = LoadTexture ("textures/12_nebula_spritesheet.png");

    const int sizeOfNebulae{5};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i=0; i<sizeOfNebulae ; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height/8;
        nebulae[i].pos.x= windowDimensions [0] + i* 300;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 1.0/16.0;
    }

    float finishLine { nebulae[sizeOfNebulae-1].pos.x };

                
    // nebula X velocity (pixels/second)
    int nebVel{-200}; // velocity for all nebula
 
    // Scarfy Variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width/6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimensions[0]/2- scarfyData.rec.width/2;
    scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0/12.0;
    scarfyData.runningTime = 0.0;


    // is the rectangle in the air
    bool isInAir{};

    // jump velocity (pixels/second)
    const int jumpVel{-600};

    int velocity{0};

    Texture2D background = LoadTexture ("textures/far-buildings.png"); // set image for background
    float bgX{};
    Texture2D midground = LoadTexture ( "textures/back-buildings.png");
    float mgX{};
    Texture2D foreground = LoadTexture ( "textures/foreground.png");
    float fgX{};

    bool collision{};

    // to freeze the backgeound after game over or game won.
    bool gameOver = false;
    bool gameWon = false;

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {

        // delta time (time since last frame)
        const float dT(GetFrameTime());
 
        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        if (!gameOver && !gameWon)
        {
        //Scroll the background
        bgX -= 20 *dT;
        if (bgX <= -background.width *2)
        {
            bgX = 0.0;
        }

        //Scroll the mid ground
        mgX -= 40 * dT;
        if (mgX <= -midground.width*2)
        {
            mgX = 0.0;
        }
    
        //Scroll the foreground
        fgX -= 80 * dT;
        if (fgX <= -foreground.width *2)
        {
            fgX= 0.0;
        }
        }
        

        // draw the background
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx( background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos{bgX + background.width * 2, 0.0};
        DrawTextureEx(background , bg2Pos, 0.0, 2.0 ,WHITE);

        // draw the midground
        Vector2 mg1Pos{mgX,0.0};
        DrawTextureEx( midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{mgX+ midground.width*2, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        //draw the foreground
        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{fgX  + foreground.width*2 ,0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0 ,WHITE);

        // perform ground check
        if ( isOnGround (scarfyData, windowDimensions[1]))
        {
            // rectangle is on the ground
            velocity = 0;
            isInAir = false;
            
        }
        else{
            // rectangle is in the air
            // apply gravity
            velocity += gravity * dT;
            isInAir = true;
        }

        // jump check
        if (IsKeyPressed(KEY_SPACE) && !isInAir )
        {
            velocity += jumpVel;
        }

        // update the position of each nebula 
        for (int i =0; i<sizeOfNebulae; i++)
        {
            nebulae[i].pos.x += nebVel * dT ;
        }

        //update finish Line
        finishLine += nebVel *dT;
        
        // update scarfy position
        scarfyData.pos.y += velocity * dT;

        // update scarfy's animation frame
        if (! isInAir)
        {
            // update running time for scarfy
            scarfyData = updateAnimData ( scarfyData, dT, 5);
            
        }

        // update nebula animation frame
        for (int i = 0; i<sizeOfNebulae; i++)
        {
           nebulae[i] = updateAnimData ( nebulae[i], dT, 7);
        }

        // Collision
       
        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x+ pad,
                nebula.pos.y+pad,
                nebula.rec.width - 2*pad,
                nebula.rec.height - 2*pad
            };
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };
            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
                gameOver =true;
            }
        }

        if (collision)
        {
            //LOSE GAME
            DrawText("Game Over!",windowDimensions[0]/4, windowDimensions[1]/2, 50 ,RED);
        }
        else if(scarfyData.pos.x >= finishLine)
        {
            //win game
            DrawText("You Win!",windowDimensions[0]/4, windowDimensions[1]/2, 50 ,GREEN);
            gameWon = true;
        }
        else
        {
            
            // Draw Nabeula
            for ( int i =0; i< sizeOfNebulae ; i++)
            {
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }

            // Draw Scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }


        // stop drawing
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}