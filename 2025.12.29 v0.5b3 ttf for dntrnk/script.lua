local tasks = require("tasks");
local frame = 0;
local whiteTex = Image.load("white.png");

-- отрисовщик текста, можешь не вникать в суть функции
local drawtext = function(text) for x = 1, 480, 60 do for y = 1, 272, 20 do LUA.print(x, y, text); end; end; end;

local alpha = 0;

local createAlphaAnimation;
---@param alphaSpeed number по скольку будет прибавляться/убавляться альфа
---@param cb? function вызовется в момент альфы == 255. функция есть - альфа повышается. функции нет - альфа понижается
createAlphaAnimation = function(alphaSpeed, cb)
    tasks.create(function()
        if (cb) then
            alpha = alpha + alphaSpeed;
        else
            alpha = alpha - alphaSpeed;
        end;

        if (alpha < 0) then alpha = 0; end;
        if (alpha > 255) then alpha = 255; end;
        if (alpha == 0) then return; end;

        if (cb and alpha >= 255) then
            cb();
            createAlphaAnimation(alphaSpeed);
        else
            createAlphaAnimation(alphaSpeed, cb);
        end;
    end, 1);
end;

local text = "gbcmrb!";

while true do
    tasks.update();
    screen.clear();

    if (frame == 100) then
        createAlphaAnimation(8, function() text = "gjgki"; end);
    end;

    if (frame == 200) then
        createAlphaAnimation(10, function() text = "ёмае"; end);
    end;

    if (frame == 300) then
        createAlphaAnimation(2, function() text = "ого"; end);
    end;

    drawtext(text);

    Image.draw(whiteTex, 0, 0, 480, 272, nil, 0, 0, 16, 16, 0, alpha);

    screen.flip();
    frame = frame + 1;
end;
