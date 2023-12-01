// searchclosest.test.js

//Step 1: Need to do: npm install --save-dev jest jsdom
//TO DO: Load HTML file into JSDOM Environment, mock "search closest doctor API call" function, set up DOM elements
//need to run npm test, add to dependencies 

const fs = require('fs');
const path = require('path');
const { JSDOM } = require('jsdom');
//only use below line if creating searchclosest.js file
//const { searchDoctor } = require('./searchclosest');

// Load the HTML content into the jsdom environment
const htmlFilePath = path.resolve(__dirname, 'searchclosest.html');
const htmlContent = fs.readFileSync(htmlFilePath, 'utf-8');
const dom = new JSDOM(htmlContent);
global.document = dom.window.document;

// Import the JavaScript logic embedded in the HTML
require('./searchclosest.html');

// Jest testing
describe('searchClosestDoctor', () => {
  test('it should display the correct information for location = 3.222_78.43', async () => {
    // Mock the fetch function to simulate API calls
    global.fetch = jest.fn(() => Promise.resolve({
      json: () => Promise.resolve({}),
      ok: true,
    }));

    // Mock DOM elements
    document.body.innerHTML = `
    <select id="sortBy">
      <option value="highestRating">By highest rating</option>
      <option value="mostRatings">By most ratings</option>
    </select>
    <input type="text" id="latitude" placeholder="Your Latitude">
    <input type="text" id="longitude" placeholder="Your Longitude">
    <button onclick="searchClosestDoctor()">Search</button>
    <div id="result"></div>
    `;

    // Set values for input fields
    document.getElementById('sortBy').value = 'location';
    document.getElementById('latitude').value = '3.222'; // Add your test values
    document.getElementById('longitude').value = '78.43'; // Add your test values

    // Trigger the searchDoctor function
    await searchClosestDoctor();

    // Check if the resultContainer content is as expected
    const resultContainer = document.getElementById('result');
    const expectedText = `Result for given latitude and longitude: {
      "id": 3,
      "doctorName": "Capybara",
      "rating": 3.5,
      "submissions": 1834,
      "location": {
        "latitude": 3.222,
        "longitude": 78.43
      },
      "practiceKeywords": ["Ear", "Nose", "Throat"],
      "languagesSpoken": ["English"],
      "Insurance": ["Aetna"],
      "Other": {
        "streetAddress": "NYC"
      }
    }`;

    expect(resultContainer.textContent).toBe(expectedText);
  });

});
