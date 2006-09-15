#	pragma once

class SoundSystemInterface;
class SoundSourceInterface;

namespace RvEngine
{
	class SoundNode
	{
	public:
		SoundNode(SoundSystemInterface* _soundSystem, SoundSourceInterface* _interface);
		~SoundNode();
	public:
		void			play(); 
		void			pause();
		void			stop();
		bool			update();
		void			setLooped(bool _flag);
		bool			getLooped() const;
		void			setVolume(float _value);
		float			getVolume() const;
		void			setPosition(const float* _position);
		const float*	getPosition() const;
		void			setDistance(float dist);
		float			getDistance() const;
		void			setHeadMode(bool flag);
		bool			getHeadMode() const;
	public:
		void			addRefCnt();
		void			releaseRefCnt();
		size_t			refCnt();
	private:
		SoundSourceInterface* m_interface;
		SoundSystemInterface* m_ssytem;
		size_t m_refCnt;
	};
}

void intrusive_ptr_add_ref(RvEngine::SoundNode *_n);
void intrusive_ptr_release(RvEngine::SoundNode *_n);