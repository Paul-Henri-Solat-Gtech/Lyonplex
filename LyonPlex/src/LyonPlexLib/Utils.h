#pragma once

#include <cmath>
#include <algorithm>
#include <random>
#include <string>
#include <chrono>
#include <fstream> 
#include <filesystem>
#include <iostream>
#include <debugapi.h>

namespace Utils 
{

    // ----------------------------------------
    // 1. Maths
    // ----------------------------------------
   
    constexpr float PI = 3.14159265358979323846f;

    // Fonction de limitation (fourchette de valeurs)
    inline float clamp(float v, float minVal, float maxVal) 
    {
        return (v < minVal) ? minVal : (v > maxVal) ? maxVal : v;
    }
    template<typename T>
    constexpr T clamp(const T& v, const T& minVal, const T& maxVal) {
        return (v < minVal) ? minVal : (v > maxVal) ? maxVal : v;
    }


    // Interpolation linÈaire
    inline float lerp(float a, float b, float t) 
    {
        return a + (b - a) * t;
    }
    template<typename T>
    constexpr T lerp(const T& a, const T& b, float t) {
        return static_cast<T>(a + (b - a) * t);
    }

    inline float smoothstep(float edge0, float edge1, float x) 
    {
        // Ajuste x dans [0,1]
        float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        // Fonction spÈcifique smooth
        return t * t * (3.0f - 2.0f * t);
    }

    inline float degToRad(float degrees) 
    {
        return degrees * (PI / 180.0f);
    }

    inline float radToDeg(float radians) 
    {
        return radians * (180.0f / PI);
    }

    // Creation du "generateur aleatoire" necessaire ÅEla generation de nombres alÈatoires
    static std::mt19937& rng() 
    {
        static std::mt19937 s_rng{ std::random_device{}() };
        return s_rng;
    }
    
    // Utilisation du "generateur aleatoire" rng() pour FLOAT
    inline float randomFloat(float minVal = 0.0f, float maxVal = 1.0f) 
    {
        std::uniform_real_distribution<float> dist(minVal, maxVal);
        return dist(rng());
    }

    // Utilisation du "generateur aleatoire" rng() pour FLOAT
    inline int randomInt(int minVal, int maxVal) 
    {
        std::uniform_int_distribution<int> dist(minVal, maxVal);
        return dist(rng());
    }


    // ----------------------------------------
    // 2. OpÈrations sur vecteurs simples (exemple Vec3)
    // ----------------------------------------
    struct Vec3 
    {
        float x, y, z;

        Vec3() : x(0), y(0), z(0) {}
        Vec3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}

        Vec3 operator+(const Vec3& o) const { return Vec3{ x + o.x, y + o.y, z + o.z }; }
        Vec3 operator-(const Vec3& o) const { return Vec3{ x - o.x, y - o.y, z - o.z }; }
        Vec3 operator*(float s)       const { return Vec3{ x * s, y * s, z * s }; }

        float length() const { return std::sqrt(x * x + y * y + z * z); }

        Vec3 normalized() const 
        {
            float len = length();
            if (len == 0.0f) return Vec3{ 0, 0, 0 };
            return (*this) * (1.0f / len);
        }
    };

    // Produit scalaire
    inline float dot(const Vec3& a, const Vec3& b) 
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    // Produit vectoriel
    inline Vec3 cross(const Vec3& a, const Vec3& b) 
    {
        return Vec3
        {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    // ----------------------------------------
    // 3. Gestion de fichiers (exemple basique)
    // ----------------------------------------
    inline bool fileExists(const std::string& path)
    {
        return std::filesystem::exists(path);
    }

    inline std::string getFileName(const std::string& fullPath) 
    {
        return std::filesystem::path(fullPath).filename().string();
    }

    inline std::string getExtension(const std::string& fullPath) 
    {
        return std::filesystem::path(fullPath).extension().string();
    }

    // Lecture simple díun fichier texte (retrourne un string complet)
    inline bool readFileToString(const std::string& path, std::string& out) 
    {
        if (!fileExists(path)) return false;
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file) return false;
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        out.resize(size);
        file.read(&out[0], size);
        file.close();
        return true;
    }

    // ----------------------------------------
    // 4. Temps / horloge
    // ----------------------------------------
    inline double getTimeSeconds() 
    {
        using namespace std::chrono;
        static auto start = high_resolution_clock::now();
        auto now = high_resolution_clock::now();
        duration<double> elapsed = now - start;
        return elapsed.count();
    }

    inline uint64_t getTimeMillis() 
    {
        using namespace std::chrono;
        static auto start = high_resolution_clock::now();
        auto now = high_resolution_clock::now();
        auto elapsed = duration_cast<milliseconds>(now - start);
        return static_cast<uint64_t>(elapsed.count());
    }

    // ----------------------------------------
    // 5. Logging basique
    // ----------------------------------------
    enum class LogLevel { Info, Warning, Error };

    inline void log(const std::string& msg, LogLevel level = LogLevel::Info) 
    {
        std::string lvl = "";
        switch (level) {
        case LogLevel::Info:
            lvl = "[INFO] ";
            break;
        case LogLevel::Warning:
            lvl = "[WARNING] ";
            break;
        case LogLevel::Error:
            lvl = "[ERROR] ";
            break;
        }
        OutputDebugStringA( (lvl + msg +"\n").c_str());
    }

    // ----------------------------------------
    // 6. Transform
    // ----------------------------------------
    
    /// Ajoute (accrÈdite) ÅEla rotation actuelle un incrÈment dÈfini
    /// par trois angles Euler (en degrÈs) autour des axes X (pitch), Y (yaw), Z (roll).

    // AddRotation<TransformComponent>(tComp, a, b, c)
    template<typename T>
    inline void AddRotation(T& t, float deltaPitchDeg, float deltaYawDeg, float deltaRollDeg)
    {
        // 1) Convertir l'incrÈment d'angles en radians
        float p = XMConvertToRadians(deltaPitchDeg);
        float y = XMConvertToRadians(deltaYawDeg);
        float r = XMConvertToRadians(deltaRollDeg);

        // 2) Construire le quaternion de l'incrÈment de rotation
        //    (Èquivalent ÅE"tourner p autour de X, puis y autour de Y, puis r autour de Z")
        XMVECTOR incQv = XMQuaternionRotationRollPitchYaw(p, y, r);

        // 3) Charger le quaternion actuel (x,y,z,w) dans un XMVECTOR
        XMVECTOR curQv = XMLoadFloat4(&t.rotation);

        // 4) Composer les deux quaternions : newQ = incQ * curQ
        //    (l'ordre importe : ici on applique incQ **avant** la rotation actuelle,
        //     ce qui correspond ÅEun repËre local).
        XMVECTOR newQv = XMQuaternionMultiply(incQv, curQv);

        // 5) Normaliser (Èviter la dÈrive numÈrique)
        newQv = XMQuaternionNormalize(newQv);

        // 6) Stocker de retour dans le composant
        XMStoreFloat4(&t.rotation, newQv);

        // 7) Marquer comme dirty pour que le TransformSystem recalcule la matrice monde
        t.dirty = true;
    }

    // SetRotation<TransformComponent>(tComp, a, b, c)
    template<typename T>
    inline void SetRotation(T& t, float pitchDeg, float yawDeg, float rollDeg)
    {
        float p = XMConvertToRadians(pitchDeg);
        float y = XMConvertToRadians(yawDeg);
        float r = XMConvertToRadians(rollDeg);
        XMVECTOR qv = XMQuaternionRotationRollPitchYaw(p, y, r);
        XMStoreFloat4(&t.rotation, qv);
        t.dirty = true;
    }



    
    // ----------------------------------------
    // 7. (…ventuel) Extensions futuresÅE
    // ----------------------------------------
    // Vous pouvez ajouter ici : gestion des threads, alignement mÈmoire, etc.
}

