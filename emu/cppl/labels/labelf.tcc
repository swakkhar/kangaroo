/*!
	@file labelf.tcc
	@brief The source file for labelf template class.
	@details This is the source file for labelf template class.
	@author M.A.Hakim Newton hakim.newton@nicta.com.au
	@author Duc Nghia Pham duc-nghia.pham@nicta.com.au
	@date 02.08.2010 QRL NICTA www.nicta.com.au
*/



#ifndef LabelfTccIncluded
#define LabelfTccIncluded



#include "cppl/roots/idx.hpp"




openEmuSpace



/*!
	@class labelf
	@brief A class to represent label template classes
			representing *flags* or labels as time-stamps.
	@details This class represents labelf template classes.
		A label is a *flag* with a time-stamp. When the time-stamp
		of a slot matches with the global time-stamp of the labelf,
		the slot is assumed to be tagged with a labelf.
*/
template <typename srl, typename mm>
class labelf
{
	public:

		typedef mm MmTyp;							//!< Memory manager type.

		labelf();									//!< Constructor.
		~labelf();									//!< Destructor.
		labelf(mm const & theMm);					//!< Initialiser.
		labelf(labelf<srl,mm> const & that);		//!< Duplicator.
		labelf<srl,mm> const & operator =
				(labelf<srl,mm> const & that);		//!< Assigner.

		//! @name Synchronise
		//! @{

		void syncClk();							//!< Synchronise clock.

		//! @}

		//! @name Clear
		//! @{

		void clear();								//!< Clear serials, retaining available memory.
		void clearMem();							//!< Clear serials, releasing memory when deemed.

		//! @}
		//! @name Insert
		//! @{

		void insert();								//!< Insert a serial at the end, assuming memory available.
		srl insertSrl();							//!< Insert a serial at the end and return the serial, assuming memory available.
		void insertMem();							//!< Insert a serial at the end, acquiring memory when needed.
		srl insertSrlMem();							//!< Insert a serial at the end and return the serial, acquiring memory available.

		//! @}

		//! @name Remove
		//! @{

		void remove();								//!< Remove the last serial, retaining available memory.
		void removeMem();							//!< Remove the last serial, releasing memroy when deemed.

		//! @}
		//! @name Memory
		//! @{

		Dim serialCount() const;					//!< Get serial count.
		mm const & MemMan() const;					//!< Memory manager.

		void reallocMemAuto();						//!< Reallocate memory automatically.
		void deallocMemAuto();						//!< Deallocate memory automatically.
		void adjustMemAuto();						//!< Adjust memory automatically.
		void allocateMem(Dim const Alloc);			//!< Allocate given extent of memory.

		//! @}
		/*! @name Operations */
		//! @{

		void sweep();									//!< Sweep tags optionally, only when required.
		void sweepMust();								//!< Sweep tags mandatorily, with a new clock.
		void sweep(Clk const theClk);					//!< Sweep tags optionally, only when requied.
		void sweepMust(Clk const theClk);				//!< Sweep tags mandatorily, with a new clock.

		void tag(srl const theSrl);							//!< Tag the serial, assuming untagged.
		void untag(srl const theSrl);						//!< Untag the serial, assuming tagged.

		Bll tagBll(srl const theSrl);						//!< Tag the serial when not tagged and return whether successful.
		Bll untagBll(srl const theSrl);						//!< Unflag the serial when not untagged and return whether successful.

		void doUntagTag(srl const theSrl1, srl const theSrl2);		//!< Untag the first and tag the second, assuming they are not already so.
		Bll doUntagTagBll(srl const theSrl1, srl const theSrl2);	//!< Untag the first and tag the second, return whether successful.

		//! @}

		/*! @name Accessors */
		//! @{

		Bll tagged(srl const theSrl) const;		//!< Whether the serial is tagged?
		Bll operator()(srl const theSrl) const;	//!< Whether the serial is tagged?
		Clk clock(srl const theSrl) const;		//!< Get the clock of a serial.
		Clk clock() const;						//!< Get the clock of the labelf.

		//!@}

	private:

		mm    mMemMan;			//!< Memory manager.
		Clk	   mClk;			//!< Current clock.
		Clk *  mClks;			//!< Srl clocks.
};



/*!
	@def lf
	@brief A macro to denote labelfs.
	@details This macro denotes labelfs.
*/
#define lf labelf



/*!
	The constructor.
*/
template<typename srl, typename mm>
inline labelf<srl,mm>::labelf() : mClk(NotClk)
{
	WatchError
	mClks = new Clk [mMemMan.acquired()];
	for (srl tSrl = 0; tSrl < mMemMan.occupied(); ++tSrl)
		mClks[tSrl] = EndClk;
	CatchError
}



/*!
	The initialiser.
*/
template<typename srl, typename mm>
inline labelf<srl,mm>::labelf(mm const & theMm) :
		mMemMan(theMm), mClk(NotClk)
{
	WatchError
	mClks = new Clk [mMemMan.acquired()];
	for (srl tSrl = 0; tSrl < mMemMan.occupied(); ++tSrl)
		mClks[tSrl] = EndClk;
	CatchError
}



/*!
	The duplicator.
*/
template <typename srl, typename mm>
labelf<srl,mm>::labelf(labelf<srl,mm> const & that) :
		mMemMan(that.mMemMan), mClk(that.mClk)
{
	WatchError
	mClks = new Clk [mMemMan.acquired()];
	for(srl tSrl = 0; tSrl < mMemMan.occupied(); ++tSrl)
		mClks[tSrl] = that.mClks[tSrl];
	CatchError
}



/*!
	The destructor.
*/
template <typename srl, typename mm>
inline labelf<srl,mm>::~labelf()
{
	WatchError
	delete [] mClks;
	CatchError
}



/*!
	The assigner.
*/
template <typename srl, typename mm>
labelf<srl,mm> const & labelf<srl,mm>::operator =
		(labelf<srl,mm> const & that)
{
	WatchError
	if (&that == this)
		return *this;

	if (mMemMan.mismatchAlloc(that.mMemMan.allocated()))
	{
		delete [] mClks;
		mClks = new Clk [that.mMemMan.acquired()];
	}

	mMemMan = that.mMemMan;
	mClk = that.mClk;

	for(srl tSrl = 0; tSrl < mMemMan.occupied(); ++tSrl)
		mClks[tSrl] = that.mClks[tSrl];

	return *this;
	CatchError
}



/*!
	Synchronise clock.
*/
template <typename srl, typename mm>
inline void labelf<srl,mm>::syncClk()
{
	WatchError

	for(srl tSrl = 0; tSrl < mMemMan.occupied(); ++tSrl)
		mClks[tSrl] = (mClks[tSrl] == mClk) ? NotClk : EndClk;
	mClk = NotClk;

	CatchError
}



/*!
	Clear serials, retaining available memory.
*/
template <typename srl, typename mm>
inline void labelf<srl,mm>::clear()
{
	WatchError

	mMemMan.resetOccupy();

	CatchError
}



/*!
	Clear serials, releasing memory when deemed.
*/
template <typename srl, typename mm>
inline void labelf<srl,mm>::clearMem()
{
	WatchError

	if (mMemMan.resetOccupyBll())
	{
		if (mMemMan.needAutoDealloc())
			deallocMemAuto();
	}

	CatchError
}



/*!
	Insert a serial at the end, assuming memory available.
*/
template <typename srl, typename mm>
inline void labelf<srl,mm>::insert()
{
	WatchError

	Warn(mMemMan.fullyOccupied(), eAllocatedMemoryFull);
	mClks[mMemMan.reoccupyItr()] = EndClk;

	CatchError
}



/*!
	Insert a serial at the end, assuming memory available.
*/
template <typename srl, typename mm>
inline void labelf<srl,mm>::insertMem()
{
	WatchError

	if (mMemMan.needAutoRealloc())
		reallocMemAuto();

	mClks[mMemMan.reoccupyItr()] = EndClk;

	CatchError
}



/*!
	Insert and return a serial at the end, assuming memory available.
*/
template <typename srl, typename mm>
inline srl labelf<srl,mm>::insertSrl()
{
	WatchError

	Warn(mMemMan.fullyOccupied(), eAllocatedMemoryFull);
	srl const tSrl = mMemMan.reoccupyItr();
	mClks[tSrl] = EndClk;
	return tSrl;

	CatchError
}




/*!
	Insert and return a serial at the end, acquiring memory when deemed.
*/
template <typename srl, typename mm>
inline srl labelf<srl,mm>::insertSrlMem()
{
	WatchError

	if (mMemMan.needAutoRealloc())
		reallocMemAuto();

	srl const tSrl = mMemMan.reoccupyItr();
	mClks[tSrl] = EndClk;
	return tSrl;

	CatchError
}



/*!
	Remove a serial from the end, retaining avaialble memory.
*/
template <typename srl, typename mm>
inline void labelf<srl,mm>::remove()
{
	WatchError

	mMemMan.unoccupy();

	CatchError
}



/*!
	Remove a serial from the end, releasing memory when needed.
*/
template <typename srl, typename mm>
inline void labelf<srl,mm>::removeMem()
{
	WatchError

	mMemMan.unoccupy();

	if (mMemMan.needAutoDealloc())
		deallocMemAuto();

	CatchError
}



/*!
	Reallocate more memory automatically.
*/
template<typename srl, typename mm>
void labelf<srl,mm>::reallocMemAuto()
{
	WatchError

	Warn(!mMemMan.needAutoRealloc(), eMemoryAllocation);
	mMemMan.reallocAuto();

	Clk * tClks = mClks;
	mClks = new Clk[mMemMan.acquired()];
	for(srl tSrl = 0; tSrl < mMemMan.occupied(); ++tSrl)
		mClks[tSrl] = tClks[tSrl];
	delete [] tClks;

	CatchError
}



/*!
	Deallocate some memory automatically.
*/
template<typename srl, typename mm>
void labelf<srl,mm>::deallocMemAuto()
{
	WatchError

	Warn(!mMemMan.needAutoDealloc(), eMemoryAllocation);
	mMemMan.deallocAuto();

	Clk * tClks = mClks;
	mClks = new Clk[mMemMan.acquired()];
	for(srl tSrl = 0; tSrl < mMemMan.occupied(); ++tSrl)
		mClks[tSrl] = tClks[tSrl];
	delete [] tClks;

	CatchError
}



/*!
	Adjust memory automatically.
*/
template<typename srl, typename mm>
void labelf<srl,mm>::adjustMemAuto()
{
	WatchError

	Warn(!mMemMan.needAutoAdjust(), eMemoryAllocation);
	mMemMan.adjustAuto();

	Clk * tClks = mClks;
	mClks = new Clk[mMemMan.acquired()];
	for(srl tSrl = 0; tSrl < mMemMan.occupied(); ++tSrl)
		mClks[tSrl] = tClks[tSrl];
	delete [] tClks;

	CatchError
}



/*!
	Allocate memory.
*/
template<typename srl, typename mm>
void labelf<srl,mm>::allocateMem(Dim const Alloc)
{
	WatchError

	Warn(!mMemMan.mismatchAlloc(Alloc), eDimensionMismatch);
	mMemMan.allocate(Alloc);

	Clk * tClks = mClks;
	mClks = new Clk[mMemMan.acquired()];
	for(srl tSrl = 0; tSrl < mMemMan.occupied(); ++tSrl)
		mClks[tSrl] = tClks[tSrl];
	delete [] tClks;

	CatchError
}



/*!
	Return the number of serial.
*/
template <typename srl, typename mm>
inline Dim labelf<srl,mm>::serialCount() const
{
	WatchError

	return mMemMan.occupied();

	CatchError
}



/*!
	Return the memory manager
*/
template <typename srl, typename mm>
inline mm const & labelf<srl,mm>::MemMan() const
{
	WatchError

	return mMemMan;

	CatchError
}



/*!
	Sweep the tags optionally, only when required.
*/
template <typename srl, typename mm>
void labelf<srl,mm>::sweep()
{
	WatchError
	if (++mClk == EndClk)
	{
		for(srl tSrl = 0; tSrl < mMemMan.occupied(); ++tSrl)
			mClks[tSrl] = (mClks[tSrl] == mClk) ? NotClk : EndClk;
		mClk = MinClk;
	}
	CatchError
}




/*!
	Sweep the tags optionally, only when required.
*/
template <typename srl, typename mm>
inline void labelf<srl,mm>::sweep(Clk const theClk)
{
	WatchError
	Warn(mClk == theClk, eClockConflict);

	mClk = theClk;
	CatchError
}



/*!
	Sweep the tags mandatorily, with a new clock.
*/
template <typename srl, typename mm>
void labelf<srl,mm>::sweepMust()
{
	WatchError
	if (++mClk == EndClk)
	{
		for(srl tSrl = 0; tSrl < mMemMan.occupied(); ++tSrl)
			mClks[tSrl] = (mClks[tSrl] == mClk) ? NotClk : EndClk;
		mClk = MinClk;
	}
	CatchError
}




/*!
	Sweep the tags mandatorily, with a new clock.
*/
template <typename srl, typename mm>
inline void labelf<srl,mm>::sweepMust(Clk const theClk)
{
	WatchError
	Warn(mClk == theClk, eClockConflict);

	mClk = theClk;
	CatchError
}



/*!
	Tag the serial, assuming untagged.
*/
template <typename srl, typename mm>
inline void labelf<srl,mm>::tag(srl const theSrl)
{
	WatchError

	Warn(mMemMan.beyondOccupied(theSrl), eSerialBeyondLimit);
	Warn(mClks[theSrl] == mClk, eClockConflict);

	mClks[theSrl] = mClk;
	CatchError
}



/*!
	Tag the serial when not untagged and return whether success
*/
template <typename srl, typename mm>
inline Bll labelf<srl,mm>::tagBll(srl const theSrl)
{
	WatchError
	Warn(mMemMan.beyondOccupied(theSrl), eSerialBeyondLimit);

	if (mClks[theSrl] == mClk)
		return false;

	mClks[theSrl] = mClk;
	return true;
	CatchError
}



/*!
	Untag the serial when not tagged.
*/
template <typename srl, typename mm>
inline void labelf<srl,mm>::untag(srl const theSrl)
{
	WatchError
	Warn(mMemMan.beyondOccupied(theSrl), eSerialBeyondLimit);
	Warn(mClks[theSrl] != mClk, eClockConflict);

	mClks[theSrl] = EndClk;
	CatchError
}



/*!
	Untag the serial when not tagged and return whether successful.
*/
template <typename srl, typename mm>
inline Bll labelf<srl,mm>::untagBll(srl const theSrl)
{
	WatchError
	Warn(mMemMan.beyondOccupied(theSrl), eSerialBeyondLimit);

	if (mClks[theSrl] != mClk)
		return false;

	mClks[theSrl] = EndClk;
	return true;
	CatchError
}



/*!
	Untag the first and tag the second, assuming they are not already so.
*/
template <typename srl, typename mm>
inline void labelf<srl,mm>::doUntagTag(srl const theSrl1, srl const theSrl2)
{
	WatchError
	Warn(theSrl1 >= mMemMan.occupied(), eSerialBeyondLimit);
	Warn(theSrl2 >= mMemMan.occupied(), eSerialBeyondLimit);
	Warn(mClks[theSrl1] != mClk, eClockConflict);
	Warn(mClks[theSrl2] == mClk, eClockConflict);

	mClks[theSrl1] = EndClk;
	mClks[theSrl2] = mClk;
	CatchError
}



/*!
	Untag the first and tag the second, return whether successful.
*/
template <typename srl, typename mm>
inline Bll labelf<srl,mm>::doUntagTagBll(srl const theSrl1, srl const theSrl2)
{
	WatchError
	Warn(theSrl1 >= mMemMan.occupied(), eSerialBeyondLimit);
	Warn(theSrl2 >= mMemMan.occupied(), eSerialBeyondLimit);

	if (mClks[theSrl1] != mClk || mClks[theSrl2] == mClk)
		return false;

	mClks[theSrl1] = EndClk;
	mClks[theSrl2] = mClk;
	return true;
	CatchError
}



/*!
	Return whether tagged?
*/
template <typename srl, typename mm>
inline Bll labelf<srl,mm>::tagged(srl const theSrl) const
{
	WatchError
	Warn(mMemMan.beyondOccupied(theSrl), eSerialBeyondLimit);

	return mClks[theSrl] == mClk;
	CatchError
}



/*!
	Return whether tagged?
*/
template <typename srl, typename mm>
inline Bll labelf<srl,mm>::operator()(srl const theSrl) const
{
	WatchError
	Warn(mMemMan.beyondOccupied(theSrl), eSerialBeyondLimit);

	return mClks[theSrl] == mClk;
	CatchError
}




/*!
	Return the clock of the given serial.
*/
template <typename srl, typename mm>
inline Clk labelf<srl,mm>::clock(srl const theSrl) const
{
	WatchError
	Warn(mMemMan.beyondOccupied(theSrl), eSerialBeyondLimit);

	return mClks[theSrl];
	CatchError
}




/*!
	Return the clock.
*/
template <typename srl, typename mm>
inline Clk labelf<srl,mm>::clock() const
{
	WatchError
	return mClk;
	CatchError
}




closeEmuSpace



#endif // LabelfTccIncluded
