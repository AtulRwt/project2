import React from 'react'

const LocationSearchPanel = ({ suggestions, setVehiclePanel, setPanelOpen, setPickup, setDestination, activeField }) => {

    const handleSuggestionClick = (suggestion) => {
        if (activeField === 'pickup') {
            setPickup(suggestion)
        } else if (activeField === 'destination') {
            setDestination(suggestion)
        }
        
        // Close the suggestions panel after selection
        setPanelOpen(false)
    }

    return (
        <div className="p-2">
            <h5 className="mb-3 text-lg font-medium">Suggestions</h5>
            {/* Display fetched suggestions */}
            {suggestions.length > 0 ? (
                suggestions.map((elem, idx) => (
                    <div 
                        key={idx} 
                        onClick={() => handleSuggestionClick(elem)} 
                        className='flex gap-4 border-2 p-3 border-gray-50 hover:border-gray-300 active:border-black rounded-xl items-center my-2 justify-start cursor-pointer'
                    >
                        <h2 className='bg-[#eee] h-8 flex items-center justify-center w-12 rounded-full'><i className="ri-map-pin-fill"></i></h2>
                        <h4 className='font-medium'>{elem}</h4>
                    </div>
                ))
            ) : (
                <div className="text-gray-500 text-center py-3">Type to see suggestions</div>
            )}
        </div>
    )
}

export default LocationSearchPanel