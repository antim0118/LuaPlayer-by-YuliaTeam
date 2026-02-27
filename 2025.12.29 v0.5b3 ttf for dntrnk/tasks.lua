-- планировщик задач
local tasks = {
    ---@type { [1]: function, [2]: number }[]
    stack = {},
};

---суть create в запуске функции cb (callback) через frames кадров. работает аналогично setTimeout в жаваскрипте
---@param cb function
---@param frames number
tasks.create = function(cb, frames)
    tasks.stack[#tasks.stack + 1] = { cb, frames };
end;

---апдейт функция. считает нужно ли в данный момент запускать callback из стека
tasks.update = function()
    for i, v in ipairs(tasks.stack) do
        v[2] = v[2] - 1;                  --вычитаем кадр
        if (v[2] <= 0) then               --если кадров 0 или меньше
            v[1]();                       --запускаем функцию
            table.remove(tasks.stack, i); --удаляем со стека
        end;
    end;
end;

return tasks;
