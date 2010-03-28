require 'test/unit'
require 'rbclips'


class Test_Complex1 < Test::Unit::TestCase

  include Clips
  include Base

  def teardown
    Base.reset
  end

  def test_main
    assert Fact.new('animal', %w(dog)).save
    assert Fact.new('animal', %w(cat)).save
    assert Fact.new('animal', %w(duck)).save
    assert Fact.new('animal', %w(turtle)).save
    assert Fact.new('warm-blooded', %w(dog)).save
    assert Fact.new('warm-blooded', %w(cat)).save
    assert Fact.new('warm-blooded', %w(duck)).save
    assert Fact.new('lays-eggs', %w(duck)).save
    assert Fact.new('lays-eggs', %w(turtle)).save
    assert Fact.new('child-of', %w(dog puppy)).save
    assert Fact.new('child-of', %w(cat kitten)).save
    assert Fact.new('child-of', %w(tutrle hatchling)).save

    assert_equal Fact.all.size, 12

    mammal1 = Rule.new "mammal" do |r|
      r.pattern 'animal', :name
      r.pattern 'warm-blooded', :name
      r.not do |n|
        n.pattern 'lays-eggs', :name
      end
      r.assert 'mammal', :name
    end
    assert mammal1.save

    mammal2 = Rule.new "childs" do |r|
      r.pattern 'mammal', :name
      r.pattern 'child-of', :name, :young
      r.assert 'mammal', :young
    end
    assert mammal2.save

    # Run the code
    Base.run

    assert_equal Fact.all.size, 16
    assert_equal Fact.find(:animal).size, 4
    assert_equal Fact.find('warm-blooded').size, 3
    assert_equal Fact.find('lays-eggs').size, 2
    assert_equal Fact.find('child-of').size, 3
    assert_equal Fact.find('mammal').size, 4
  end
end
