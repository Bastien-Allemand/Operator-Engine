#pragma once
class IComponentPool
{
public:
    virtual ~IComponentPool() = default;
	virtual void Remove(uint32 _entity) = 0;
};

template<typename T>
class ComponentPool : public IComponentPool
{
public:
    ComponentPool();
	~ComponentPool() = default;

    T* AddComponent(uint32 _entity);
	T* GComponent(uint32 _entity);
	void Remove(uint32 _entity) override;

    Vector<uint32> GEntityIds();
private:
	Vector<T> m_components;
	Vector<uint32> m_index; // ex : m_index[0] = entity id at same position in m_components m_components[0] is the component of entity with id m_index[0]
};

template<typename T>
inline ComponentPool<T>::ComponentPool()
{
    m_components.reserve(4096);
    m_index.reserve(4096);
}

template<typename T>
inline T* ComponentPool<T>::AddComponent(uint32 _entity)
{
    //check if exists
    for (size_t i = 0; i < m_index.size(); i++)
        if (m_index[i] == _entity)
        {
			return &m_components[i];
        }
	T newComponent;
    int indexpos = -1;

    for (size_t i = 0; i < m_index.size(); i++)
        if (m_index[i] == MAX_VALUE)
        {
			indexpos = i;
            break;
        }
    if(indexpos == -1)
    {
        m_components.push_back(newComponent);
        m_index.push_back(_entity);
		indexpos = m_index.size() - 1;
    }
    else
    {
        m_components[indexpos] = newComponent;
        m_index[indexpos] = _entity;
	}
	return &m_components[indexpos];
}

template<typename T>
inline T* ComponentPool<T>::GComponent(uint32 _entity)
{
    for (size_t i = 0; i < m_index.size(); i++)
        if (m_index[i] == _entity)
        {
            return &m_components[i];
        }
	return nullptr;
}

template<typename T>
inline void ComponentPool<T>::Remove(uint32 _entity)
{
    for (size_t i = 0; i < m_index.size(); i++)
        if (m_index[i] == _entity)
        {
            m_index[i] = MAX_VALUE;
			m_components[i] = T();
            return;
		}
}
template<typename T>
Vector<uint32> ComponentPool<T>::GEntityIds()
{
    Vector<uint32> entityIds;
    for (size_t i = 0; i < m_index.size(); i++)
        if (m_index[i] != MAX_VALUE)
            entityIds.push_back(m_index[i]);
	return entityIds;
}
