#ifndef PACMAN_RESOURCE_MANAGER_HPP
#define PACMAN_RESOURCE_MANAGER_HPP

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <cassert>
#include <initializer_list>

namespace pacman {

enum class TextureID {
    Pacman,
    Map,
    Ghost,
    PacmanDeath
};

enum class SoundBufferID {
    Wa,
    Ka,
    EatGhost,
    Death1,
    Death2,
    GameStart,
    Intermission,
    Energizer,
    RareLoss,
    GhostSiren
};

enum class FontID {
    EmuLogic
};

template <typename ResourceType, typename IDType>
class ResourceManager {
public:
    ResourceManager(::std::initializer_list< ::std::pair<IDType,::std::string>> list)
    {
        for (const auto& pair : list)
            load(pair.first,pair.second);
    }
    
    ResourceType& load(IDType id, const ::std::string& file_path) 
    {
        auto texture = std::make_unique<ResourceType>();

        if (!texture->loadFromFile(file_path))
            assert(!"texture couldn't be loaded!");
        
        auto iterator = mResourceMap.insert(
            std::make_pair(id, std::move(texture)
        ));

        assert( "texture loaded twice!" && iterator.second);
        return *iterator.first->second;
    }
    ResourceType& get(IDType id) noexcept 
    {
        const auto found = mResourceMap.find(id);
        assert("texture not loaded" && found != mResourceMap.end());
        return *found->second;
    }
    const ResourceType& get(IDType id) const noexcept
    {
        const auto found = mResourceMap.find(id);
        assert("texture not loaded" && found != mResourceMap.end());
        return *found->second;
    }

    ResourceType& operator[](IDType id) noexcept 
    {
        const auto found = mResourceMap.find(id);
        assert("texture not loaded" && found != mResourceMap.end());
        return *found->second;
    }

    const ResourceType& operator[](IDType id) const noexcept 
    {
        const auto found = mResourceMap.find(id);
        assert("texture not loaded" && found != mResourceMap.end());
        return *found->second;
    }
private:
    using map_type = std::unordered_map<IDType, std::unique_ptr<ResourceType>>;
    map_type mResourceMap;
};

using TextureManager = ResourceManager<sf::Texture, TextureID>;
using SoundBufferManager = ResourceManager<sf::SoundBuffer, SoundBufferID>;
using FontManager = ResourceManager<sf::Font, FontID>;
}

#endif // !defined(PACMAN_RESOURCE_MANAGER_HPP)