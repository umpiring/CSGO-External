#include "memory.h"
#include "Offsets.h"
#include "imgui.h"
#include "config.h"


#include <sstream>
//..



struct Vector {
    Vector() noexcept
        : x(), y(), z() {}

    Vector(float x, float y, float z) noexcept
        : x(x), y(y), z(z) {}

    Vector& operator+(const Vector& v) noexcept {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector& operator-(const Vector& v) noexcept {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    float x, y, z;
};

template<typename T>
std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

struct ViewMatrix {
    ViewMatrix() noexcept
        : data() {}

    float* operator[](int index) noexcept {
        return data[index];
    }

    const float* operator[](int index) const noexcept {
        return data[index];
    }

    float data[4][4];
};
static bool world_to_screen(const Vector& world, Vector& screen, const ViewMatrix& vm) noexcept {
    float w = vm[3][0] * world.x + vm[3][1] * world.y + vm[3][2] * world.z + vm[3][3];

    if (w < 0.001f) {
        return false;
    }

    const float x = world.x * vm[0][0] + world.y * vm[0][1] + world.z * vm[0][2] + vm[0][3];
    const float y = world.x * vm[1][0] + world.y * vm[1][1] + world.z * vm[1][2] + vm[1][3];

    w = 1.f / w;
    float nx = x * w;
    float ny = y * w;

    const ImVec2 size = ImGui::GetIO().DisplaySize;

    screen.x = (size.x * 0.5f * nx) + (nx + size.x * 0.5f);
    screen.y = -(size.y * 0.5f * ny) + (ny + size.y * 0.5f);

    return true;
}
namespace boxESP {
    void boxEsp()
    {

        auto mem = Memory{ "csgo.exe" };
        const auto client = mem.GetModuleAddress("client.dll");
        const auto engine = mem.GetModuleAddress("engine.dll");

        const auto local_player = mem.Read<uintptr_t>(client + Offsets::dwLocalPlayer);


        for (auto i = 1; i <= 32; ++i)
        {
            const auto view_matrix = mem.Read<ViewMatrix>(client + Offsets::dwViewMatrix);
            const auto player = mem.Read<DWORD>(client + Offsets::dwEntityList + i * 0x10);

            const auto bones = mem.Read<DWORD>(player + Offsets::m_dwBoneMatrix);


            if (mem.Read<int>(player + Offsets::m_iHealth) > 0)
            {
                Vector head_pos{
                    mem.Read<float>(bones + 0x30 * 8 + 0x0C),
                    mem.Read<float>(bones + 0x30 * 8 + 0x1C),
                    mem.Read<float>(bones + 0x30 * 8 + 0x2C)
                };

                Vector top;
                Vector bottom;



                const auto EntityP = player;

                Vector local_pos{
                    mem.Read<float>(local_player + Offsets::m_vecOrigin),
                    mem.Read<float>(local_player + Offsets::m_vecOrigin + 0x4),
                    mem.Read<float>(local_player + Offsets::m_vecOrigin + 0x8),
                };

                Vector entity_pos{
                    mem.Read<float>(EntityP + Offsets::m_vecOrigin),
                    mem.Read<float>(EntityP + Offsets::m_vecOrigin + 0x4),
                    mem.Read<float>(EntityP + Offsets::m_vecOrigin + 0x8),
                };

                auto feet_pos = mem.Read<Vector>(player + Offsets::m_vecOrigin);

                float height = abs(head_pos.y - entity_pos.y);
                float width = height * 0.65;

                if (Config::toggleBoxes)
                {
                    if (world_to_screen(head_pos + Vector{ 0, 0, 11.f }, top, view_matrix) && world_to_screen(feet_pos - Vector{ 0, 0, 7.f }, bottom, view_matrix)) {


                        const float h = bottom.y - top.y;
                        const float w = h * 0.35f;

                        ImColor boxCol = { Vars::boxCol[0], Vars::boxCol[1], Vars::boxCol[2] };


                        ImGui::GetBackgroundDrawList()->AddRect({ top.x - w, top.y }, { top.x + w, bottom.y }, boxCol, 10000.f, NULL, Vars::boxThick);

                    }
                }
                
                class player_info {
                private:
                    char __pad[0x10];
                public:
                    char name[32];
                };

                const auto client_state = mem.Read<DWORD>(engine + Offsets::dwClientState);
                const auto user_info_table = mem.Read<DWORD>(client_state + Offsets::dwClientState_PlayerInfo);
                const auto items = mem.Read<int>(user_info_table + 0x40) + 0xC;
                player_info Tes5t = mem.Read<player_info>(mem.Read<DWORD>((items + 0x28) + (i * 0x34)));

                const char* player_name = Tes5t.name; // your name is here
                float _dist = (float)trunc(sqrt(pow((entity_pos.x - local_pos.x), 2) + pow((entity_pos.y - local_pos.y), 2)) / 100);

                if (Config::toggleHealth)
                {
                    if (world_to_screen(head_pos + Vector{ 0, 0 , 0.f }, top, view_matrix) && world_to_screen(feet_pos - Vector{ 0, 0 , 0.f }, bottom, view_matrix))
                    {

                        const float h = bottom.y - top.y;
                        const float w = h * 0.35f;
                        const float distance = _dist * 0.03f;
                        const int test = static_cast<int>(1.5 / sqrt(distance));
                        const int Health = mem.Read<int>(player + Offsets::m_iHealth);

                        ImGuiIO& io = ImGui::GetIO();
                        io.Fonts->AddFontFromFileTTF("C:\Windows\Fonts\vgafix.ttf", 15);
                        ImFont* Small = io.Fonts->AddFontFromFileTTF("C:\Windows\Fonts\vgafix.ttf", 15);

                        std::string s = std::to_string(Health);
                        //   std::string s = std::to_string(distance);
                        //std::string s = player_name;
                        char const* pchar = s.c_str();

                        //const ImVec2 text_pos = ImVec2(top.x - w, top.y + h);


                        const ImVec2 text_pos = ImVec2((top.x - w), top.y + h);
                        ImColor textCol = { Vars::textCol[0], Vars::textCol[1], Vars::textCol[2] };


                        ImGui::GetBackgroundDrawList()->AddText(Small, static_cast<int>(6 / sqrt(distance)), text_pos, textCol, pchar);


                    }
                }
                
            }
        }
    }

}
