#	pragma once

#	include "TextField.h"

class	SoundSourceInterface;
class	SoundBufferInterface;

namespace	Menge
{
	class	ResourceSound;

	class SoundTextField
		: public TextField
	{
		OBJECT_DECLARE(SoundTextField);
	public:
		SoundTextField();

	public:
		const Message * start() override;

	protected:
		void	_update(float _timing) override;

	private:
		SoundBufferInterface * m_sampleMusic;
		SoundSourceInterface * m_soundSourceInterface;

		void	_prepareSound(const Message * _message);
	};
}